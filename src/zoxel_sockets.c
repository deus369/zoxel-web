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
#define windows_zip_file_name "zoxel_windows_x86_64.zip"
#define linux_zip_file_name "zoxel_linux_x86_64.zip"
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

void send_client_message(int sock, const char *buffer) {
#ifdef _WIN32
    send(sock, buffer, (int) strlen(buffer), 0);
#else
    write(sock, buffer, strlen(buffer));
#endif
}

int send_redirect(int client_sock, const char *url) {
    // Send HTTP redirect headers
    char http_response[256];
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 303 See Other\r\n"
             "Location: %s\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<html><body><h1>File Downloaded. Redirecting...</h1></body></html>", url);
    send(client_sock, http_response, strlen(http_response), 0);
    return 0;   // no error
}

int send_file(int client_sock, const char *file_path, const char *filename) {
    FILE *file;
    char buffer[1024];
    size_t file_size, read_size;
    // Open the file to be served
    file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        close_client_socket(client_sock);
        return 1;
    }
    // Get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // Send HTTP headers
    char http_response[256];
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/octet-stream\r\n"
             "Content-Disposition: attachment; filename=\"%s\"\r\n"
             "Content-Length: %zu\r\n"
             "\r\n", filename, file_size);
    send(client_sock, http_response, strlen(http_response), 0);
    // Send file content
    while ((read_size = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_sock, buffer, read_size, 0);
    }
    // Close the file
    fclose(file);
    return 0;   // no error
}

int send_html_response(int client_sock, const char *html_content) {
    char http_response[1024];
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "\r\n%s", strlen(html_content), html_content);
    send(client_sock, http_response, strlen(http_response), 0);
    return 0;   // no error
}

int respond_to_client(int client_sock, char *client_message, int read_size) {
    client_message[read_size] = '\0';
    zox_log(" > Sending response to client");
    // Parse the HTTP request line to extract the URL
    char method[16], url[256], protocol[16];
    if (sscanf(client_message, "%s %s %s", method, url, protocol) == 3) {
        zox_log(" - Method: %s", method);
        zox_log(" - URL: %s", url);
        zox_log(" - Protocol: %s", protocol);
        // Check if the URL is "/extensions"
        // if file URL: /favicon.ico -> send back file
        if (strcmp(url, "/") == 0) {
            send_client_message(client_sock, response);
        } else if (strcmp(url, windows_zip_file_name) == 0) {
            if (send_file(client_sock, windows_zip_file_name, windows_zip_file_name)) {
                zox_log(" ! Error sending build file [%s]", windows_zip_file_name);
            }
        } else if (strcmp(url, linux_zip_file_name) == 0) {
            if (send_file(client_sock, linux_zip_file_name, windows_zip_file_name)) {
                zox_log(" ! Error sending build file [%s]", linux_zip_file_name);
            }
        } else if (strcmp(url, "/play") == 0) {
            // Send a simple HTTP response
            const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\ntodo: live zoxel here\n";
            send_client_message(client_sock, response);
        } else {
            // for 404 we just send them back to main website
            // Send a 404 Not Found response
            // const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nPage not found\n";
            // send_client_message(client_sock, response);
            send_client_message(client_sock, response);
        }
    } else {
        // send_client_message(client_sock, response);
        // Send a 400 Bad Request response
        const char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid request\n";
        send_client_message(client_sock, response);
    }
}

// debug check if this stalls
int accept_incoming_connections(int web_sock, char *response) {
    struct sockaddr_in client;
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

    num_clients++;
    char *client_ip = inet_ntoa(client.sin_addr);
    zox_log(" > Connection accepted from [%s] num [%i]", client_ip, num_clients);
    is_dirty = 1;

    // Receive a message from client
    char client_message[2000];
    int read_size = recv(client_sock, client_message, sizeof(client_message) - 1, 0);
    zox_log(" > Received %d bytes from client [%i]", read_size, client_sock);
    if (read_size >= 0) {
        respond_to_client(client_sock, client_message, read_size);
    } else if (read_size == -1) {
        if (!is_terminal_ui) perror("   ! client message failed");
        // send this back anyway?
        // send_client_message(client_sock, response);
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
