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

// Prints menu and returns the selection
// TODO: Remove 'exit' from here and make it a global option you can do from any
// where in the program
char menu() {
	while (true) {
		printf("Please select an option:\n");
		printf("(1) Get next ascii:\n");
		printf("(2) Exit:\n");
		char input = getchar();
		clear_input_buffer();
		if (input == '\n') {
			printf("No input provided\n\n");
		}
		else if (input != '1' && input != '2') {
			printf("Invalid input: %c\n\n", input);
		}
		else return input;
	}
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
    // "127.0.0.1" is a universal standard called the loopback address. It means
	// localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
	// Outer loop to retry if server disconnects
	while (true) {
		// Loop to keep trying to connect to server
		printf("Attempting to connect to server...\n");
		while (true) {
			if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))
					>= 0) {
				break;
				//perror("connection failed");
				//exit(EXIT_FAILURE);
			}
		}
		// TODO: Implement menu items
		printf("Connected to server!\n");
		while (true) {
			char selection = menu();
			if (selection == '1') {
				next_ascii(sock, valread, buffer);
			}
			if (selection =='2') {
				printf("Exiting...\n");
				close(sock);
				return 0;
			}
		}
	}
}
