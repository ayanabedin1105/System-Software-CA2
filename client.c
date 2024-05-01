#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], filename[100], directory[20], username[20], password[20];
    FILE *file;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        return 1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
    printf("Connected to server\n");

    // Get and send authentication details
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    snprintf(message, BUFFER_SIZE, "%s %s", username, password);
    if (send(sock, message, strlen(message), 0) < 0) {
        puts("Send failed");
        return 1;
    }

    // Get file and directory information
    printf("Enter the filename: ");
    scanf("%s", filename);
    printf("Enter directory (Manufacturing or Distribution): ");
    scanf("%s", directory);
    snprintf(message, BUFFER_SIZE, "%s %s", directory, filename);
    if (send(sock, message, strlen(message), 0) < 0) {
        puts("Send failed");
        return 1;
    }

    // Open file and send contents
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }
    while (!feof(file)) {
        int bytes_read = fread(message, 1, BUFFER_SIZE, file);
        if (send(sock, message, bytes_read, 0) < 0) {
            puts("Failed to send file");
            break;
        }
    }
    fclose(file);

    close(sock);
    return 0;
}
