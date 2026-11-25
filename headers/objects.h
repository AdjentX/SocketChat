#pragma once

#ifndef OBJECTS_H
#define OBJECTS_H

#include <sqlite3.h>

typedef enum {
    ROLE_REGULAR = 0,
    ROLE_VIP = 1,
    ROLE_HOME = 2,
    ROLE_ADMIN = 3
} role_t;

typedef enum {
    DUPLICATE_USER = 0,
    USER_NOT_FOUND = 1,
    INVALID_LOGIN = 2
} UserErrorsDB;

typedef enum {
    CHANGE_PASS = 8,
    CHANGE_PLAN = 9,
    TOGGLE_ACCOUNT = 7
} ClientModification;

typedef enum {
    ADD_USER = 0,
    DELETE_USER = 1,
    UPDATE_USER = 2,
    VIEW_LOGS = 3,
    LOGIN = 4,
} UserOperationType;

typedef struct {
    role_t role;
    char *str;
    int conns;
} _role_map_t;

typedef struct {
    sqlite3 *db_handler;
} Database;

typedef struct {
    int userID;
    role_t userRoles;
    bool userDisabled;
    int maxConns;
} _user_acc_t;

typedef struct {
    char username[255];
    char password[255];
    char *ip_address;
    int attacks_active;
    bool logged_in;
    _user_acc_t cli_acc;
} ClientInfo;

typedef struct {
    struct sockaddr_in *client_addr;
    char console_buff[2048];
    int client_fd;
} HandlerInfo;

#endif // OBJECTS_H