/* I am going to release next update soon which will add support for musl (remove fstab.h and change it to my own implementation of it). I am working on it and I plan to release this within one week. */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include <fstab.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

// Trim leading/trailing whitespace and certain punctuation characters
// Removes spaces, colons, and semicolons from both ends of a string
static void trim_inplace(char *s) {
  if (!s) return;

  char *start = s;
  while (*start && isspace((unsigned char)*start))
    start++;
  if (start != s)
    memmove(s, start, strlen(start) + 1);

  size_t len = strlen(s);
  while (len > 0) {
    unsigned char c = (unsigned char)s[len - 1];
    if (isspace(c) || c == ':' || c == ';')
      s[--len] = '\0';
    else
      break;
  }
}

// Simple string equality check
// Returns true if both strings are non-NULL and identical
static bool streq(const char *a, const char *b) {
  return a && b && strcmp(a, b) == 0;
}

// Check if a specific option exists exactly in a comma-separated list of options
static bool has_option_exact(const char *opts, const char *option) {
  if (!opts || !option) return false;
  char *copy = strdup(opts);
  if (!copy) return false;

  bool found = false;
  for (char *tok = strtok(copy, ","); tok; tok = strtok(NULL, ",")) {
    trim_inplace(tok); // Remove whitespace and punctuation
    if (*tok == '\0') continue;
    if (strcmp(tok, option) == 0) { found = true; break; }
  }
  free(copy);
  return found;
}

// Parse mount options string into kernel flags and extra filesystem-specific options
static unsigned long parse_mount_options(const char *opts, char **extra_opts_out) {
  unsigned long flags = 0;
  char *extra = NULL;
  size_t extra_len = 0;

  if (!opts) { *extra_opts_out = NULL; return 0; }

  char *copy = strdup(opts);
  if (!copy) { *extra_opts_out = NULL; return 0; }

  // Iterate over comma-separated mount options
  for (char *tok = strtok(copy, ","); tok; tok = strtok(NULL, ",")) {
    trim_inplace(tok);
    if (*tok == '\0') continue;

    // Map known mount options to kernel flags
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
      // 'defaults' option does not modify flags
    } else {
      // Unknown options are collected into a comma-separated string for fs-specific data
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

// Resolve UUID= or LABEL= filesystem specifiers to actual device paths
static const char* resolve_fs_spec(const char* fs_spec) {
  static char buf[PATH_MAX];
  if (!fs_spec) return NULL;

  // Translate UUID= or LABEL= to /dev/disk/by-uuid/ or /dev/disk/by-label/
  if (strncmp(fs_spec, "UUID=", 5) == 0) {
    snprintf(buf, sizeof(buf), "/dev/disk/by-uuid/%s", fs_spec + 5);
  } else if (strncmp(fs_spec, "LABEL=", 6) == 0) {
    snprintf(buf, sizeof(buf), "/dev/disk/by-label/%s", fs_spec + 6);
  } else {
    return fs_spec;
  }

  // Resolve symlink to real device path
  char real[PATH_MAX];
  ssize_t len = readlink(buf, real, sizeof(real)-1);
  if (len <= 0) return fs_spec;
  real[len] = '\0';

  const char *base = strrchr(real, '/');
  if (!base) return fs_spec;
  snprintf(buf, sizeof(buf), "/dev/%s", base + 1);
  return buf;
}

// Read /etc/fstab and mount entries that do not have "noauto"
int stage0(void) {
  setfsent();  // Open fstab
  struct fstab *fs;
  while ((fs = getfsent()) != NULL) {
    // Skip entries with "noauto" option
    if (has_option_exact(fs->fs_mntops, "noauto"))
      continue;

    struct stat st;
    // Ensure the mount point directory exists
    if (stat(fs->fs_file, &st) != 0 || !S_ISDIR(st.st_mode)) {
      fprintf(stderr, "Skipping %s: mount point is invalid\n", fs->fs_file);
      continue;
    }

    printf("Found device:\n  fs_spec: %s\n  fs_file: %s\n  fs_vfstype: %s\n  opts: %s\n",
           fs->fs_spec, fs->fs_file, fs->fs_vfstype, fs->fs_mntops ? fs->fs_mntops : "NULL");

    char *extra_opts = NULL;
    unsigned long flags = parse_mount_options(fs->fs_mntops, &extra_opts);
    const void *data_arg = (extra_opts && *extra_opts) ? extra_opts : NULL;

    const char *dev = resolve_fs_spec(fs->fs_spec);
    if (mount(dev, fs->fs_file, fs->fs_vfstype, flags, data_arg) == -1) {
      fprintf(stderr, "Error mounting %s on %s: %s\n", dev, fs->fs_file, strerror(errno));
    } else {
      printf("Successfully mounted %s on %s\n", dev, fs->fs_file);
    }

    free(extra_opts);
    putchar('\n');
  }
  endfsent();  // Close fstab
  return 0;
}
