#include <fcntl.h>      // for fcntl function
#include <errno.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    // #include <netinet/in.h>
    // #include <netdb.h>
#endif

int num_clients = 0;
int web_sock;
int does_socket_exist = 0;
// remember: browser uses port 80
int server_port = 80; // im rerouting this traffic from 80 to 8080 internally
int is_minify = 1;
char* html_index = "index.html";
char* html_index_minify = "index_mini.html";
char* response = NULL;
int has_loaded_html = 0;
// char *inline_response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Website home was not found.</h1></body></html>\n";

void close_socket() {
    if (does_socket_exist) {
        close(web_sock);
        does_socket_exist = 0;
    }
}

unsigned char make_socket_non_blocking(int web_sock) {
#ifdef _WIN32
    u_long mode = 1;
    if (ioctlsocket(web_sock, FIONBIO, &mode) != NO_ERROR) {
        fprintf(stderr, " ! ioctlsocket failed: %d\n", WSAGetLastError());
        return 0;
    }
    zox_log(" + success, socket non blocking");
#else
    int flags = fcntl(web_sock, F_GETFL, 0);
    if (flags == -1) {
        zox_log(" ! failed making non block");
        if (!is_terminal_ui) perror("fcntl");
        return 0;
    }
    flags |= O_NONBLOCK;
    if (fcntl(web_sock, F_SETFL, flags) == -1) {
        zox_log(" ! failed making non block");
        if (!is_terminal_ui) perror("fcntl");
        return 0;
    }
#endif
    return 1;
}

int initialize_sockets() {
    if (does_socket_exist) return 0;
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
#endif
    struct sockaddr_in server;
    web_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (web_sock == -1) {
        fprintf(stderr, " ! could not create socket: %s\n", strerror(errno));
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(server_port);
    if (bind(web_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        zox_log(" ! bind failed [%i]", server_port);
        perror("    ! error: ");
        return 1;
    }
    if (!make_socket_non_blocking(web_sock)) {
        zox_log(" ! socket blocks");
        return 1;
    }
    does_socket_exist = 1;
    // Retrieve and print the IP address
    if (!is_terminal_ui) {
        char ip_str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &server.sin_addr, ip_str, sizeof(ip_str)) == NULL) {
            fprintf(stderr, " ! could not convert IP address to string: %s\n", strerror(errno));
            return 1;
        }
        zox_log(" + socket initialized at IP [%s] and port [%i]", ip_str, server_port);
    }
    return 0;
}

void start_listening() {
    zox_log(" > starting listening at [%i]", web_sock);
    listen(web_sock, 3);
    zox_log(" > started listening");
}

#ifdef _WIN32
// Helper function to get WSA error string
const char* wsa_strerror(int err) {
    static char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   buf, sizeof(buf), NULL);
    return buf;
}
#endif

void close_client_socket(
#ifdef _WIN32
    SOCKET sock
#else
    int sock
#endif
) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

// debug check if this stalls
int accept_incoming_connections(int web_sock, char *response) {
    struct sockaddr_in client;
    char client_message[2000];
    int c = sizeof(struct sockaddr_in);

    // Accept connection from an incoming client
#ifdef _WIN32
    SOCKET client_sock = accept(web_sock, (struct sockaddr *)&client, &c);
    if (client_sock == INVALID_SOCKET) {
#else
    int client_sock = accept(web_sock, (struct sockaddr *) &client, (socklen_t *) &c);
    if (client_sock < 0) {
#endif
        int error =
#ifdef _WIN32
            WSAGetLastError();
#else
            errno;
#endif
        close_client_socket(client_sock);
        if (!(error == EAGAIN || error == EWOULDBLOCK || 10035 == error || error == 11)) {
            zox_log(" ! accept failed [%i]", error);
            if (!is_terminal_ui) {
                fprintf(stderr, " ! accept failed: [%i] %s\n", error,
#ifdef _WIN32
                    wsa_strerror(error));
#else
                    strerror(error));
#endif
            }
            return 1;
        } else {
            // zox_log(" > no client [%i]", error);
            return 0;
        }
    }

    char *client_ip = inet_ntoa(client.sin_addr);
    zox_log(" > Connection accepted from [%s]", client_ip);
    num_clients++;
    is_dirty = 1;

    // Receive a message from client
#ifdef _WIN32
    int read_size = recv(client_sock, client_message, sizeof(client_message) - 1, 0);
#else
    int read_size = recv(client_sock, client_message, sizeof(client_message) - 1, 0);
#endif
    zox_log(" > Received %d bytes from client", read_size);
    if (read_size >= 0) {
        zox_log(" > Sending response to client");
#ifdef _WIN32
        send(client_sock, response, (int) strlen(response), 0);
#else
        write(client_sock, response, strlen(response));
#endif
    } else if (read_size == -1) {
        if (!is_terminal_ui) perror("recv failed");
    }

    // Close the client socket
    close_client_socket(client_sock);

    return 0;   // no error
}

char* build_http_response(const char* html_file_buffer) {
    char* response_output = malloc(strlen(html_file_buffer) * sizeof(char) + 256);
    sprintf(response_output, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n%s\n", html_file_buffer);
    return response_output;
}

int load_html() {
    char* html_file = load_file(html_index);
    if (html_file == NULL) return 1;
    if (has_loaded_html) {
        free(response);
    } else {
        has_loaded_html = 1;
    }
    response = build_http_response(html_file);
    free(html_file);
    return 0;
}

void update_html(char* html_filepath) {
    if (was_modified(html_filepath)) {
        if (load_html()) {
            set_mod_time(html_filepath);   // set file as dirty only when successfully loads
        }
    }
}

void update_web() {
    // zox_log(" > accept_incoming_connections [%i]", server_port);
    accept_incoming_connections(web_sock, response);
}

int open_web() {
    if (initialize_sockets()) return 1;
    if (is_minify) set_mod_time(html_index_minify);
    else set_mod_time(html_index);
    load_html();
    zox_log(" + opened web hosting");
    return 0;
}

void close_web() {
    close_socket();
    if (has_loaded_html) {
        free(response);
        has_loaded_html = 0;
    }
}
