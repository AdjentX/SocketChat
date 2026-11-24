#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <sqlite3.h>

#include "utils.h"

#define DB_FILE "qbot-v2.sql"

typedef enum {
    ROLE_REGULAR = 0,
    ROLE_VIP = 1,
    ROLE_HOME = 2,
    ROLE_ADMIN = 3
} role_t;

typedef struct {
    role_t role;
    char *str;
    int conns;
} _role_map_t;

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
    sqlite3 *db_handler;
} Database;

typedef struct {
    int *userID;
    role_t userRoles;
    bool userDisabled;
    int maxConns;
} UserAccount;

extern Database our_db;

#endif // DATABASE_H