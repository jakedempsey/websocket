#include <stdio.h>
#include <stdlib.h>           // Standard library functions
#include <unistd.h>           // UNIX standard function definitions
#include <string.h>           // String manipulation functions
#include <sys/socket.h>       // Socket programming functions
#include <netinet/in.h>       // Internet address family functions
#include <stdbool.h>       // Internet address family functions

#define PORT 8080            // Port number for the server
#define BUFFER_SIZE 1      // Size of the buffer for sending/receiving data

struct sockaddr_in get_address_struct() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;          // Set address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);        // Convert port number to network byte order
    return address;
}

int get_listening_socket() {
    int server_fd;            // File descriptors for the server socket and new connections
    int opt = 1;                           // Option value for setsockopt()
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");           // Print error message if socket creation fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");              // Print error message if setsockopt() fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    return server_fd;
}

void bind_socket_to_address(struct sockaddr_in address, int server_fd) {
    int addrlen = sizeof(address);         // Length of the address structure
    if (bind(server_fd, (struct sockaddr *)&address, addrlen)<0) {
        perror("bind failed");              // Print error message if binding fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
}

int accept_client(struct sockaddr_in address, int server_fd) {
    printf("Accepting...\n");
    int new_socket;
    int addrlen = sizeof(address);         // Length of the address structure
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");                   // Print error message if accept() fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    return new_socket;
}

void do_session(int new_socket) {
    while (true) {
        char buffer[BUFFER_SIZE] = {0}; // Should cause an error, no?
        printf("Reading...\n");
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            printf("An error ocurred while reading the message. Going back to accepting state.\n");
            break;
        }
        if (bytes_read == 0) {
            printf("Client disconnected. Going back to accepting state.\n");
            break;
        }
        //TODO: Interesting stuff here. It's all nicely abstracted now
        printf("Received: %c\n", buffer[0]); 
        char* response = &buffer[0];
        printf("Returning: %c\n", ++*response);
        send(new_socket, response, 1, 0);
    }
}

int main() {
    int server_fd = get_listening_socket();
    struct sockaddr_in address = get_address_struct();
    bind_socket_to_address(address, server_fd);
    while (true) {
        printf("Listening...\n");
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        int new_socket = accept_client(address, server_fd);
        do_session(new_socket);
    }
    printf("Exiting...");
    return 0;
}
