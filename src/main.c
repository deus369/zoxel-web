// === zoxel web ===
//
//  > to test go to 192.68.0.1:8080
//
// disable on windows
#ifdef _WIN32
    #define zox_disable_terminal
#endif
#define sleep_time 1
#include <stdio.h>
#include <string.h>// extern void zox_log(char *text, ...);
// my scripts
#include "logs.c"
#ifndef zox_disable_terminal
    #include "zoxel_terminal.c"
    #include "zoxel_input.c"
#else
    unsigned char is_terminal_ui = 0;
    int is_dirty = 0;
#endif
#include "zoxel_io.c"
#include "zoxel_sockets.c"
// windows.h after sockets includes
#ifdef _WIN32
#include <windows.h>
#define sleep(ms) Sleep(ms)
#endif
// #include <vte/vte.h> // for VteTerminal
// #define include_user_input
// #define live_html_data

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
    if (!is_terminal_ui) return;
    if (!is_dirty) return;
    is_dirty = 0;
#ifndef zox_disable_terminal
    clear_terminal();
    char text[1024] = ""; // initialize to empty string
    add_text(text, "Zoxel Web [%i]\n", server_port);
    add_text(text, "\n");
    add_text(text, "Clients Connected [%d]\n", num_clients);
    add_text(text, "\n");
    add_text(text, "Press q to exit.\n");
    display_centered_text(text);
    fflush(stdout); // flush the output buffer
#endif
}

void set_text(char* display_text) {
    if (!is_terminal_ui) {
        printf("%s\n", display_text);
        return;
    }
#ifndef zox_disable_terminal
    clear_terminal();
    printf("\033[?25l"); // hide the cursor
    printf("\033[31m"); // set red text color
    char text[1024] = ""; // initialize to empty string
    add_text(text, display_text);
    display_centered_text(text);
    fflush(stdout); // flush the output buffer
#endif
}

void handle_events() {
#ifndef _WIN32
    signal(SIGINT, on_interupt_app);
    signal(SIGWINCH, handle_sigwinch);
#endif
}

void begin_open_app() {
#ifndef zox_disable_terminal
    if (is_terminal_ui) {
        save_terminal_settings();
        save_terminal_state();
        set_text("Loading Zoxel Web\n");
    } else {
        save_terminal_settings_log();
    }
#endif
}

void open_app() {
    sleep(sleep_time);
    begin_open_app();
    float total_time = 0;
    while (open_web()) {
        char text[256];
        sprintf(text, "Opening Socket [%i] at [%f]", server_port, total_time);
        set_text(text);
        zox_log(text);
        sleep(sleep_time);
        total_time += sleep_time;
    }
    start_listening();
    is_dirty = 1;
}

void close_app() {
    close_web();
#ifndef zox_disable_terminal
    restore_terminal_settings();
    if (is_terminal_ui) if (exit_reason != 255) clear_terminal();
#endif
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
    sleep(sleep_time);
    open_app();
}

int parse_int(const char *str, int default_value) {
    return (str && *str) ? (int)strtol(str, NULL, 10) : default_value;
}

void process_arguments(int argc, char *argv[]) {
    zox_log(" > arguments [%i]", argc);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log") == 0) is_terminal_ui = 0;
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            server_port = parse_int(argv[i + 1], 8080);
            zox_log(" + port set to %i", server_port);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            zox_log(" > zoxel web help menu");
            zox_log("   -> -l --log for normal console output");
            zox_log("   -> -p --port to set a port number");
            exit(0);
        }
    }
}

void quit_app() {
    running = 0;
    exit_reason = 1;
}

void toggle_minify() {
    is_minify = !is_minify;
    if (is_minify) set_text("Toggled Minify [ON]\n");
    else set_text("Toggled Minify [OFF]\n");
    sleep(sleep_time);
    restart_app();
}

void handle_user_input() {
#ifndef zox_disable_terminal
    char key_press = get_keyboard_key(is_terminal_ui);
    if (key_press == 'q') quit_app();
    else if (key_press == 'r') restart_app();
    else if (key_press == 'm') toggle_minify();
#endif
}

int main(int argc, char *argv[]) {
    zox_log(" > started main");
    process_arguments(argc, argv);
    handle_events();
    open_app();
    zox_log(" > running loop");
    while (running) {
#ifdef live_html_data
        if (is_minify) update_html(html_index_minify);
        else update_html(html_index);
#endif
        update_web();
        display_text();
#ifdef include_user_input
        handle_user_input();
#endif
    }
    close_app();
    return 0;
}
