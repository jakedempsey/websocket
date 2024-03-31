#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// TODO: Tidy up user input, only accept alphanumerial characters.
// TODO: Abstract some of the code into functions.
// TODO: Try to abstract some of the web code. It's all pass by reference
//       and heavily dependent on above imports, so might be difficul.
int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "a";
    int input;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        printf("Input a char:\n");
        input = getchar();

        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (input == 'z') {
            printf("Special character 'z' passed, exiting...");
            break;
        }

        // Send char to server
        send(sock, &input, 1, 0);

        // Receive response from server
        valread = read(sock, buffer, BUFFER_SIZE);
        printf("Value from server: %c\n", buffer[0]);
    }

    // Close connection
    close(sock);
    return 0;
}
