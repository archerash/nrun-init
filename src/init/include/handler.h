#ifndef _HANDLER_H
#define _HANDLER_H

void sigterm_handler(int signum);
void sigint_handler(int signum);
void sigchld_handler(int signum);

#endif
