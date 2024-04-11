#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool is_valid_input(char c) {
    bool is_number = c >= 48 && c <= 57;
    bool is_uppercase = c >= 65 && c <= 90;
    bool is_lowercase = c >= 97 && c <= 122;
    if (is_lowercase || is_uppercase || is_number) {
        return true;
    }
    else return false;
}

void next_ascii(int sock, int valread, char buffer[]) {
    int input;
    while (true) {
        printf("Input a char:\n");
        input = getchar();
        if (input == '\n') {
            printf("No input provided\n\n");
            continue;
        }
        clear_input_buffer();
        if (!is_valid_input(input)) {
            printf("Invalid input: %c\n\n", input);
            continue;
        }
        if (input == '6') {
            printf("Special character '6' passed, exiting...\n");
            break;
        }

        // Send char to server
        send(sock, &input, 1, 0);

        // Receive response from server
        valread = read(sock, buffer, BUFFER_SIZE);
        printf("Value from server: %c\n\n", buffer[0]);
    }
}

int menu() {
    printf("Please select an option:\n");
    printf("(1) N ascii:\n");
    printf("(2) Unimplemented:\n");
}

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
    // "127.0.0.1" is a universal standard called the loopback address. It means localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }
    // TODO: Implement menu items
    while (true) {
        switch(menu()){
            case('1'):
                next_ascii(sock, valread, buffer);
            case('2'):
                prinf("Not implemented");
        }

        next_ascii(sock, valread, buffer);
        break;
    }

    close(sock);
    return 0;
}
