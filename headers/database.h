#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DB_FILE "qbot-v2.sql"
#include "objects.h"

int user_manager(UserOperationType opt_type, void *args);
extern Database our_db;

#endif // DATABASE_H