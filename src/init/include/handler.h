#ifndef _HANDLER_H
#define _HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t shutdown_r;
extern volatile sig_atomic_t reboot_r;

void sigterm_handler(int signum);
void sigint_handler(int signum);
void sigchld_handler(int signum);
void handle_pending_actions(void);

#endif
