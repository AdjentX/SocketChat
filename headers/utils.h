#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include "database.h"

typedef struct {
    char username[255];
    char password[255];
    char *ip_address;
    int attacks_active;
    bool logged_in;
    UserAccount acc_info;
} ClientInfo;

#include "handler.h"
#include "worker.h"

// returns remote ip str in readable text form from passed fd.
char *get_socket_ip(int sock, struct sockaddr_in addr);

// strip buffer leftovers.
void strip_input(char *buffer, int *sz);

// listner that spawns worker threads for every accepted new accept() returned client_fd.
void *create_and_bind(int port, int max_clients);

int set_nonblocking(int fd);

// title writer.
ssize_t set_terminal_title(int fd, const char *title);

// initializes a server fd for handler create_and_bind().
int init_server_fd();

#endif // UTILS_H