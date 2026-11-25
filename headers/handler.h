#pragma once
#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "objects.h"

// set terminal title.
ssize_t set_terminal_title(int fd, const char *title);

// cleans up finished workers.
void handle_client_exit(HandlerInfo *, ClientInfo *);

#endif // HANDLER_H
