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

struct User {
    char username[20];
    char password[20];
};

struct User users[] = {
    {"user1", "pass1"},
    {"user2", "pass2"},
    {"user3", "pass3"}
};

int authenticate(char *username, char *password) {
    for (int i = 0; i < sizeof(users)/sizeof(users[0]); i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
            return 1; // Authentication successful
    }
    return 0; // Authentication failed
}

void log_transfer(const char *username, const char *filename, const char *directory) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    char *dt = ctime(&now);
    fprintf(log, "%s: %s transferred '%s' to '%s'\n", dt, username, filename, directory);
    fclose(log);
}

void *client_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char client_message[BUFFER_SIZE], username[20], password[20], filename[100], directory[20], filepath[150];
    FILE *file;
    int read_size;

    memset(client_message, 0, BUFFER_SIZE); // Clear the buffer

    // First, receive and authenticate user
    if ((read_size = recv(sock, client_message, BUFFER_SIZE - 1, 0)) > 0) {
        client_message[read_size] = '\0'; // Null-terminate the string
        sscanf(client_message, "%s %s", username, password);
        if (!authenticate(username, password)) {
            printf("Authentication failed for user %s\n", username);
            close(sock);
            free(socket_desc);
            return 0;
        }
        printf("User %s authenticated successfully.\n", username);
    } else {
        close(sock);
        free(socket_desc);
        return 0; // Handle errors or disconnections
    }

    memset(client_message, 0, BUFFER_SIZE); // Clear the buffer again after authentication

    // Receive file transfer data
    if ((read_size = recv(sock, client_message, BUFFER_SIZE - 1, 0)) > 0) {
        client_message[read_size] = '\0'; // Null-terminate the string
        sscanf(client_message, "%s %s", directory, filename);
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, filename);
        file = fopen(filepath, "wb");
        while ((read_size = recv(sock, client_message, BUFFER_SIZE, 0)) > 0) {
            fwrite(client_message, 1, read_size, file);
        }
        fclose(file);
        log_transfer(username, filename, directory);  // Log the transfer
    }

    close(sock);
    free(socket_desc);
    return 0;
}

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return 1;
    }

    listen(socket_desc, 3);
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }

        pthread_t sniffer_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if(pthread_create(&sniffer_thread, NULL, client_handler, (void*) new_sock) < 0) {
            perror("could not create thread");
            continue;
        }
        pthread_detach(sniffer_thread);
    }

    return 0;
}
