#pragma once
#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <sys/socket.h>

#include "utils.h"

typedef struct {
    struct sockaddr_in *client_addr;
    char console_buff[2048];
    int client_fd;
} HandlerInfo;

// cleans up finished workers.
void handle_client_exit(HandlerInfo *, ClientInfo *);

#endif // HANDLER_H
