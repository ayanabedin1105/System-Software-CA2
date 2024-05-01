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
    char buffer[BUFFER_SIZE];
    char filename[100];
    char directory[20];
    char user[10];
    FILE *file;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected to server\n");

    // Get user identity, file, and directory from user
    printf("Enter your user identity (user1, user2, user3): ");
    scanf("%s", user);
    printf("Enter the filename: ");
    scanf("%s", filename);
    printf("Enter directory (manufacturing/ distribution): ");
    scanf("%s", directory);

    // Send user identity, filename, and directory to server
    snprintf(buffer, sizeof(buffer), "%s %s %s", user, directory, filename);
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        puts("Send failed");
        return 1;
    }

    // Open file
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // Read file contents into buffer and send
    while (!feof(file)) {
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        if (send(sock, buffer, bytes_read, 0) < 0) {
            puts("Failed to send file");
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    puts("File sent successfully");

    close(sock);
    return 0;
}
