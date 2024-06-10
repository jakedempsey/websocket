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

// Does nothing, handles argv if needed in future
void printArgvArray(int argc, char* argv[]) {
	printf("\n--------Printing passed arguments (currently unused...)\n\n");
    for (int i = 0; i < argc; i++) {
	    printf("Argument %d: ", i);
	    for (int j = 0; argv[i][j] != '\0'; j++) {
		    printf("%c", argv[i][j]);
	    }
		printf("\n");
    }
	printf("\n");
}

int main(int argc, char* argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "a";
    int input;

	//printArgvArray(argc, argv);

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

    // Try to connect to server
	printf("Attempting to connect to server...\n");
	while (true) {
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0) {
			break;
			//perror("connection failed");
			//exit(EXIT_FAILURE);
		}
	}
    // TODO: Implement menu items
	printf("Connected to server!\n");
    while (true) {
        switch(menu()){
            case('1'):
                next_ascii(sock, valread, buffer);
				break;
            case('2'):
                printf("Not implemented");
				break;
			default:
				printf("Nothing selected. Defaulting to ascii selection\n");
				next_ascii(sock, valread, buffer);
        }
    }

    close(sock);
    return 0;
}
