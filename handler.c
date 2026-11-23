#include "headers/handler.h"

void handle_client_exit(HandlerInfo *this_handler, ClientInfo *this_client) {
    set_terminal_title(this_handler->client_fd, "Auth Failed");

    send(this_handler->client_fd, "\033[2J\033[H", 7, MSG_NOSIGNAL);
    send(this_handler->client_fd, "Unathorized: your socket is being closed!", 16, MSG_NOSIGNAL);
    sleep(4);
    close(this_handler->client_fd);
    free(this_handler);
    free(this_client);
}