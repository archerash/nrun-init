#ifndef _SIG_ATOMIC_T_H___
#define _SIG_ATOMIC_T_H___

#include <signal.h>

extern volatile sig_atomic_t shutdown_r;
extern volatile sig_atomic_t reboot_r;

#endif
