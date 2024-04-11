#include <stdio.h>
#include <stdlib.h>           // Standard library functions
#include <unistd.h>           // UNIX standard function definitions
#include <string.h>           // String manipulation functions
#include <sys/socket.h>       // Socket programming functions
#include <netinet/in.h>       // Internet address family functions
#include <stdbool.h>       // Internet address family functions

#define PORT 8080            // Port number for the server
#define BUFFER_SIZE 1      // Size of the buffer for sending/receiving data

int main() {
    int server_fd, new_socket;             // File descriptors for the server socket and new connections
    struct sockaddr_in address;            // Structure to hold server address information
    int opt = 1;                           // Option value for setsockopt()
    int addrlen = sizeof(address);         // Length of the address structure
    char buffer[BUFFER_SIZE] = {0};        // Buffer for sending/receiving data
    const char *hello = "Hello from server"; // Message to send to the client
    // Create (listening) socket file descriptor
    // socket() tells the OS to allocate resources for a socket. The int return value
    // is the unique address for that socket.
    // server_fd is a LISTENING socket, it only accepts incoming connections and assigns the
    // a unique socket.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");           // Print error message if socket creation fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    // Attach socket to port 8080 and enable address reuse
    // setsockopt sets options for a socket.
    // server_fd is the socket
    // SOL_SOCKET means we're setting the option at socket level
    // SO_REUSEADDR is the option: make local addresses 'resuable'. This is because the OS
    // blocks a port from other bindings for a short period after a socket is closed. This
    // prevents conflicts with other processes. It's mostly for rapid-starting your server.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");              // Print error message if setsockopt() fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    address.sin_family = AF_INET;          // Set address family to IPv4
    // INADDR_ANY allows connections from any interface, since a machine might have several ways
    // that it's connected to a network. For a client, this isn't super relevant, as it includes
    // localhost 127.0.0.1, which is where the client is calling from. This would become relevant
    // when I want to connect to this server from a different machine. It would then listen on
    // the laptops network card. The loopback address is hosted locally, and doesnt touch any
    // network intefaces. It is special network interface that is internal to the OS.
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);        // Convert port number to network byte order
    // Bind socket to the specified address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");              // Print error message if binding fails
        exit(EXIT_FAILURE);                // Exit the program with failure status
    }
    // TODO: Idea. For now, understand how client-server works step-by-step
    // TODO: Quick refresher on structs
    while (true) {
        printf("Listening...\n");
        // listen on this socket with a queue of up to 3 connections. When the client calls the
        // request first goes to this queue.
        if (listen(server_fd, 3) < 0) {
            perror("listen");                   // Print error message if listen() fails
            exit(EXIT_FAILURE);                // Exit the program with failure status
        }
        // Block until a new connection is made
        printf("Accepting...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");                   // Print error message if accept() fails
            exit(EXIT_FAILURE);                // Exit the program with failure status
        }
        while (true) {
            printf("Reading...\n");
            // Block until more data is sent
            int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
            if (bytes_read < 0) {
                printf("An error ocurred while reading the message!");
            }
            if (bytes_read == 0) {
                printf("Client disconnected. Going back to accepting state.\n");
                break;
            }
            printf("Received: %c\n", buffer[0]); 
            char* response = &buffer[0];
            printf("Returning: %c\n", ++*response);
            send(new_socket, response, 1, 0);
        }
    }
    printf("Exiting...");
    return 0;
}
