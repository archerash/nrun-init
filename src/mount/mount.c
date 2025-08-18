#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#include "include/fstab.h"
#include "include/msg.h"
#include "include/streq.h"

// Trim leading and trailing whitespace and some punctuation from string in-place
static void trim_inplace(char *s) {
  if (!s) return;
  char *start = s;
  while (*start && isspace((unsigned char)*start)) start++; // skip leading spaces
  if (start != s) memmove(s, start, strlen(start) + 1); // shift string left

  size_t len = strlen(s);
  while (len > 0) {
    unsigned char c = (unsigned char)s[len - 1];
    if (isspace(c) || c == ':' || c == ';') s[--len] = '\0'; // remove trailing spaces/:/;
    else break;
  }
}

// Check if mount options string contains exactly the specified option
static bool has_option_exact(const char *opts, const char *option) {
  if (!opts || !option) return false;
  char *copy = strdup(opts); // duplicate string to tokenize
  if (!copy) return false;
  bool found = false;
  for (char *tok = strtok(copy, ","); tok; tok = strtok(NULL, ",")) {
    trim_inplace(tok); // remove whitespace and trailing chars
    if (*tok == '\0') continue;
    if (strcmp(tok, option) == 0) { found = true; break; } // exact match
  }
  free(copy);
  return found;
}

// Parse mount options into flags and extra options for mount()
static unsigned long parse_mount_options(const char *opts, char **extra_opts_out) {
  unsigned long flags = 0; // mount flags
  char *extra = NULL; // extra options string
  size_t extra_len = 0;
  if (!opts) { *extra_opts_out = NULL; return 0; }

  char *copy = strdup(opts);
  if (!copy) { *extra_opts_out = NULL; return 0; }

  for (char *tok = strtok(copy, ","); tok; tok = strtok(NULL, ",")) {
    trim_inplace(tok);
    if (*tok == '\0') continue;

    // Map known option strings to MS_* flags
    if (streq(tok, "ro")) flags |= MS_RDONLY;
    else if (streq(tok, "rw")) flags &= ~MS_RDONLY;
    else if (streq(tok, "nosuid")) flags |= MS_NOSUID;
    else if (streq(tok, "nodev")) flags |= MS_NODEV;
    else if (streq(tok, "noexec")) flags |= MS_NOEXEC;
    else if (streq(tok, "sync")) flags |= MS_SYNCHRONOUS;
    else if (streq(tok, "remount")) flags |= MS_REMOUNT;
#ifdef MS_MANDLOCK
    else if (streq(tok, "mand")) flags |= MS_MANDLOCK;
#endif
#ifdef MS_DIRSYNC
    else if (streq(tok, "dirsync")) flags |= MS_DIRSYNC;
#endif
    else if (streq(tok, "noatime")) flags |= MS_NOATIME;
    else if (streq(tok, "nodiratime")) flags |= MS_NODIRATIME;
#ifdef MS_BIND
    else if (streq(tok, "bind")) flags |= MS_BIND;
#endif
#ifdef MS_SHARED
    else if (streq(tok, "shared")) flags |= MS_SHARED;
#endif
#ifdef MS_PRIVATE
    else if (streq(tok, "private")) flags |= MS_PRIVATE;
#endif
#ifdef MS_SLAVE
    else if (streq(tok, "slave")) flags |= MS_SLAVE;
#endif
#ifdef MS_UNBINDABLE
    else if (streq(tok, "unbindable")) flags |= MS_UNBINDABLE;
#endif
#ifdef MS_RELATIME
    else if (streq(tok, "relatime")) flags |= MS_RELATIME;
#endif
#ifdef MS_STRICTATIME
    else if (streq(tok, "strictatime")) flags |= MS_STRICTATIME;
#endif
    else if (streq(tok, "norelatime")) {
#ifdef MS_RELATIME
      flags &= ~MS_RELATIME;
#endif
    }
    else if (streq(tok, "defaults")) {
      // defaults has no effect
    } else {
      // Treat unknown option as extra string to pass to mount()
      size_t toklen = strlen(tok);
      size_t need = extra_len + (extra_len ? 1 : 0) + toklen + 1;
      char *tmp = realloc(extra, need);
      if (!tmp) { free(extra); extra = NULL; extra_len = 0; break; }
      extra = tmp;
      if (extra_len == 0) {
        memcpy(extra, tok, toklen);
        extra[toklen] = '\0';
        extra_len = toklen;
      } else {
        extra[extra_len] = ',';
        memcpy(extra + extra_len + 1, tok, toklen);
        extra[extra_len + 1 + toklen] = '\0';
        extra_len += 1 + toklen;
      }
    }
  }

  free(copy);
  *extra_opts_out = (extra_len ? extra : NULL);
  if (!extra_len && extra) free(extra);
  return flags;
}

// Resolve UUID= or LABEL= in fstab to /dev device path
static const char* resolve_fs_spec(const char* fs_spec) {
  static char buf[PATH_MAX];
  if (!fs_spec) return NULL;

  if (strncmp(fs_spec, "UUID=", 5) == 0) snprintf(buf, sizeof(buf), "/dev/disk/by-uuid/%s", fs_spec + 5);
  else if (strncmp(fs_spec, "LABEL=", 6) == 0) snprintf(buf, sizeof(buf), "/dev/disk/by-label/%s", fs_spec + 6);
  else return fs_spec;

  char real[PATH_MAX];
  ssize_t len = readlink(buf, real, sizeof(real)-1); // follow symlink to actual device
  if (len <= 0) return fs_spec;
  real[len] = '\0';
  const char *base = strrchr(real, '/'); // get basename of device path
  if (!base) return fs_spec;
  snprintf(buf, sizeof(buf), "/dev/%s", base + 1); // return /dev/sdX style path
  return buf;
}

// Main function to mount filesystems from fstab
int mount_devices(void) {
  int fd = open("/etc/fstab", O_RDONLY); // open fstab file
  if (fd == -1) return 1;

  char buf[128];
  ssize_t n;
  int lines = 0;
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (ssize_t i = 0; i < n; i++) if (buf[i] == '\0') lines++; // count lines
  }

  if (n == -1) { close(fd); return 1; }

  int i = 1;
  struct fstab entry;
  while (readfstab(fd, i, &entry) == 0) { // read fstab entries
    i++;
    if (has_option_exact(entry.mntopts, "noauto")) continue; // skip noauto mounts

    struct stat st;
    if (stat(entry.mnt, &st) != 0 || !S_ISDIR(st.st_mode)) continue; // skip non-directories

    char *extra_opts = NULL;
    unsigned long flags = parse_mount_options(entry.mntopts, &extra_opts); // parse mount options
    const void *data_arg = (extra_opts && *extra_opts) ? extra_opts : NULL;
    const char *dev = resolve_fs_spec(entry.uuid); // resolve UUID/LABEL to /dev path

    // Debug info
    printf("New device found: %s\n"
           " UUID: %s\n"
           " Mountpoint: %s\n"
           " Filesystem: %s\n"
           " Options: %s\n"
           " Dump: %d\n"
           " Pass: %d\n", dev, entry.uuid, entry.mnt, entry.fstype, entry.mntopts, entry.fs_dump, entry.fs_pass);

    // Attempts mounting
    if (mount(dev, entry.mnt, entry.fstype, flags, data_arg) == -1) {
      errmsg("Failed to mount %s on %s: %s\n", dev, entry.mnt, strerror(errno)); 
    }
    free(extra_opts);
  }

  close(fd); // Close fstab
  return 0;
}

int main(void) {
  // Starts mounting, you can change this core-service with simple shell script:

  /* #!/bin/sh
     mount -a  */
  printf("-> Mounting devices\n");
  mount_devices();
}
