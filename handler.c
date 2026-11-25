#include "headers/handler.h"

ssize_t set_terminal_title(int fd, const char *title) {
    char buf[256];
    int len = snprintf(buf, sizeof(buf), "\033]0;%s\007", title);

    if (len < 0 || (size_t)len >= sizeof(buf))
        return -1; // Title was too long or formatting failed

    return send(fd, buf, (size_t)len, MSG_NOSIGNAL);
}

void handle_client_exit(HandlerInfo *this_handler, ClientInfo *this_client) {
    set_terminal_title(this_handler->client_fd, "Auth Failed");

    send(this_handler->client_fd, "\033[2J\033[H", 7, MSG_NOSIGNAL);
    send(this_handler->client_fd, "Unathorized: your socket is being closed!", 16, MSG_NOSIGNAL);
    sleep(4);
    close(this_handler->client_fd);
    free(this_handler);
    free(this_client);
}