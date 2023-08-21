#include <stdio.h>
#include <string.h>
#include <signal.h>     // for signal function
#include <sys/ioctl.h>  // for struct winsize and the TIOCGWINSZ 
#include "zoxel_terminal.c"
#include "zoxel_input.c"
#include "zoxel_io.c"
#include "zoxel_sockets.c"
// #include <vte/vte.h> // for VteTerminal

// variables
int running = 1;
int exit_reason = 0;

void on_interupt_app(int sig) {
    running = 0;
    exit_reason = 2;
}

void handle_sigwinch(int sig) {
    is_dirty = 1;   // refresh ui when terminal resizes
}

void display_text() {
    if (!is_dirty) return;
    is_dirty = 0;
    clear_terminal();
    char text[1024] = ""; // initialize to empty string
    add_text(text, "Zoxel Web [%i]\n", server_port);
    add_text(text, "\n");
    add_text(text, "Clients Connected [%d]\n", num_clients);
    add_text(text, "\n");
    add_text(text, "Press q to exit.\n");
    display_centered_text(text);
    fflush(stdout); // flush the output buffer
}

void set_text(char* display_text) {
    if (!is_terminal_ui) {
        puts(display_text);
        return;
    }
    clear_terminal();
    printf("\033[?25l"); // hide the cursor
    printf("\033[31m"); // set red text color
    char text[1024] = ""; // initialize to empty string
    add_text(text, display_text);
    display_centered_text(text);
    fflush(stdout); // flush the output buffer
}

void handle_events() {
    signal(SIGINT, on_interupt_app);
    signal(SIGWINCH, handle_sigwinch);
}

void begin_open_app() {
    if (is_terminal_ui) {
        save_terminal_settings();
        save_terminal_state();
        set_text("Loading Zoxel Web\n");
    } else {
        save_terminal_settings_log();
    }
}

void open_app() {
    begin_open_app();
    while (open_web()) {
        set_text("Attempting to open socket");
        sleep(3);
    }
    sleep(1);
    start_listening();
    is_dirty = 1;
}

void close_app() {
    close_web();
    restore_terminal_settings();
    if (is_terminal_ui) {
        if (exit_reason != 255) clear_terminal();
    }
    if (exit_reason == 1) {
        printf("Exited app due to q key was pressed.\n");
    } else if (exit_reason == 2) {
        printf("Exited app due to control + C was pressed.\n");
    } else if (exit_reason == 3) {
        printf("Failed to open app.\n");
    }
}

void restart_app() {
    close_app();
    set_text("Restarting Zoxel Web");
    sleep(3);
    open_app();
}

void process_arguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log") == 0) {
            is_terminal_ui = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    process_arguments(argc, argv);
    handle_events();
    open_app();
    while (running) {
        if (is_terminal_ui) {
            display_text();
        }
        update_web();
        char key_press = get_keyboard_key(is_terminal_ui);
        if (key_press == 'q') {
            running = 0;
            exit_reason = 1;
        } else if (key_press == 'r') {
            restart_app();
        } else if (key_press == 'm') {
            is_minify = !is_minify;
            if (is_minify)
                set_text("Toggled Minify [ON]\n");
            else
                set_text("Toggled Minify [OFF]\n");
            sleep(3);
            restart_app();
        }
    }
    close_app();
    return 0;
}