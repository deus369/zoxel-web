#include <stdlib.h> // for system function
#include <stdarg.h> // for va_list, va_start, and va_end
extern unsigned char is_terminal_ui;

void zox_log(char *text, ...) {
    if (is_terminal_ui) return;
    va_list list;
    va_start(list, text);
    char msg2[256] = { 0 };
    vsnprintf(msg2, sizeof(msg2), text, list);
    printf("%s\n", msg2);
    va_end(list);
    fflush(stdout);
}
