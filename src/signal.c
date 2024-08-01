#ifndef _WIN32
#include <signal.h>

extern int running;
extern int exit_reason;
extern unsigned char is_dirty;

void on_interupt_app(int sig) {
    running = 0;
    exit_reason = 2;
}

void handle_sigwinch(int sig) {
    is_dirty = 1;   // refresh ui when terminal resizes
}

void handle_events() {
    signal(SIGINT, on_interupt_app);
    signal(SIGWINCH, handle_sigwinch);
}

#else

void handle_events() { }

#endif
