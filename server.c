#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define LOG_FILE "transfer.log"

void log_transfer(const char *user, const char *filename, const char *directory) {
    FILE *log = fopen(LOG_FILE, "a");  // Open the log file in append mode
    if (log == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';  // Remove the newline character from ctime output

    fprintf(log, "[%s] %s transferred '%s' to '%s'\n", time_str, user, filename, directory);
    fclose(log);
}

void *client_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char client_message[BUFFER_SIZE];
    char filename[100], directory[20], user[10], filepath[150];
    FILE *file;
    int read_size;

    // Read user, directory, and filename from the client
    if ((read_size = recv(sock, client_message, BUFFER_SIZE, 0)) > 0) {
        sscanf(client_message, "%s %s %s", user, directory, filename);
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, filename);

        printf("%s is transferring the file '%s' to the directory '%s'\n", user, filename, directory);
        log_transfer(user, filename, directory);  // Log the transfer

        file = fopen(filepath, "wb");
        if (file == NULL) {
            fprintf(stderr, "Error opening file: %s (Errno: %d)\n", strerror(errno), errno);
            close(sock);
            free(socket_desc);
            return 0;
        }

        // Continue reading data and write to file
        while ((read_size = recv(sock, client_message, BUFFER_SIZE, 0)) > 0) {
            fwrite(client_message, 1, read_size, file);
        }

        fclose(file);
        if (read_size == 0) {
            puts("File received and saved successfully");
        } else if (read_size == -1) {
            perror("recv failed");
        }
    }

    close(sock);
    free(socket_desc);
    return 0;
}

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return 1;
    }
    puts("Socket created");

    // Set up the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind the socket
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return 1;
    }
    puts("Bind done");

    // Listen for connections
    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }
        puts("Connection accepted");

        pthread_t sniffer_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, client_handler, (void*) new_sock) < 0) {
            perror("could not create thread");
            continue;
        }
        pthread_detach(sniffer_thread);  // Automatically free resources upon thread completion
    }

    return 0;
}
