#include "headers/database.h"

Database our_db;

// enum, str, int
static const _role_map_t roles_arr[] = {
    { ROLE_ADMIN, "ADMIN", -1 },
    { ROLE_VIP, "VIP", 2 },
    { ROLE_REGULAR, "STANDARD", 1 },
    { ROLE_HOME, "HOME", 5 }
};

int user_manager(UserOperationType opt_type, void *args) {
    sqlite3_stmt *stmt = NULL;
    int result;
    char *sql;

    switch (opt_type) {
        case LOGIN:
            ClientInfo *cli_info = (ClientInfo *)args;
            // login query prepared statement.
            sql = "SELECT id, password, username, max_conns, role FROM accounts WHERE username = ?;";
            if (sqlite3_prepare_v2(our_db.db_handler, (const char *)sql, -1, &stmt, NULL) != SQLITE_OK) {
                fprintf(stderr, "Error: %s\n", sqlite3_errmsg(our_db.db_handler));
                return -1;
            }

            sqlite3_bind_text(stmt, 1, cli_info->username, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const char *stored_pass = (const char *)sqlite3_column_text(stmt, 1);
                if (stored_pass && !strcmp(stored_pass, cli_info->password)) {
                    cli_info->logged_in = true;
                    cli_info->acc_info.userID = sqlite_column_int(stmt, 0);
                    cli_info->acc_info.maxConns = sqlite3_column_int(stmt, 3);
                    cli_info->acc_info.userRole = (role_t)sqlite_colun_int(stmt, 4);

                    sqlite3_finalize(stmt);
                    return 0;
                } else {
                    // invalid pass.
                    sqlite3_finalize(stmt);
                    return -1;
                } 
            } else {
                // invalid username. (no user found by said alias.)
                sqlite3_finalize(stmt);
                return -1;
            }
        break;

        case ADD_USER:
            char **new_user = (char **)args;

            char *user = new_user[0];
            char *pass = new_user[1];
            role_t role = atoi(new_user[3]);
            int max_conns = atoi(new_user[2]);

            if (strlen(new_user) < 4) {
                fprintf(stderr, "ERROR: database.c -> user_manager(AddUser) - missing parameters!\n");
                return -1;
            }

            // add user prepared statement.
            sql = "INSERT INTO  accounts (username, password, max_conns, role) VALUES (?, ?, ?, ?);";
            if (sqlite3_prepare_v2(our_db.db_handler, (const char *)sql, -1, &stmt, NULL) != SQLITE_OK) {
                fprintf(stderr, "Error: %s\n", sqlite3_errmsg(our_db.db_handler));
                return -1;
            }

            sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, max_conns);
            sqlite3_bind_int(stmt, 4, role);

            result = sqlite3_step(stmt);
            if (result = SQLITE_DONE) {
                sqlite3_finalize(stmt);
                return 0;
            } else if (result == SQLITE_CONSTRAINT) {
                fprintf(stderr, "Error adding user: %s\n", sqlite3_errmsg(our_db.db_handler));
                sqlite3_finalize(stmt);
                return -1;
            }
            fprintf(stderr, "ERROR: database.c -> user_manager(AddUser) - failure: %s", sqlite3_errmsg(our_db.db_handler));
            sqlite3_finalize(stmt);
            return -1;
        break;

        case DELETE_USER:
            char **del_user = (char **)args;
            
            if (strlen(del_user) <= 0) {
                fprintf(stderr, "ERROR: database.c -> user_manager(DELETE_USER) requires the userID");
                return -1;
            }
            int user_id = atoi(del_user[0]);
            sql = "DELETE FROM accounts WHERE id = ?";
            // rest of the logic to delete acc, had to go to slave job...

    }  
}

/*
int add_user(char *user, char *pass, int max_conns, AvailableRoles role) {
    const char *sql = "INSERT INTO accounts (username, password, max_conns, role) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(our_db.db_handler, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "database.c -> add_user(): Error preparing statement!\nContext: %s\n", sqlite3_errmsg(our_db.db_handler));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, max_conns);
    sqlite3_bind_int(stmt, 4, role);

    int result = sqlite3_step(stmt);
    if (result == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return 0;
    } else if (result == SQLITE_CONSTRAINT) {
        fprintf(stderr, "Error: %s", error_to_str(usersErrors));
        sqlite3_finalize(stmt);
        return 1; // Duplicate user entry already found.
    }

    fprintf(stderr, "database.c -> add_user(): Execution failed!\nContext: %s\n", sqlite3_errmsg(our_db.db_handler));
    sqlite3_finalize(stmt);
    return -1;
}

int delete_user(int userID) {
    const char *delete_user_by_id_sql = "DELETE FROM accounts WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(our_db.db_handler, delete_user_by_id_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "database.c -> delete_user(), Error preparing delete_user\nContext: %s\n", sqlite3_errmsg(our_db.db_handler));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, userID);
    int deletion_attempt = sqlite3_step(stmt);

    if (deletion_attempt == SQLITE_ROW) {
        char *userAlias = (char*)sqlite3_column_text(stmt, 0);
        fprintf(stdout, "User deleted => Alias: %s - ID: %d\n", userAlias, userID);
        return 0; // success
    } 
    else if (deletion_attempt != SQLITE_DONE)
        fprintf(stderr, "database.c -> delete_user(), Execution failed: %s\n", sqlite3_errmsg(our_db.db_handler));
    else
        fprintf(stdout, "database.c -> delete_user(), user not found by submitted ID %d\n", userID);

    sqlite3_finalize(stmt);
    return -1;
}

*/

int init_db(const char *db_file) {
    const char *init_tables[] = {
        // accounts table
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT NOT NULL UNIQUE,"
        "  password TEXT NOT NULL,"
        "  max_conns INTEGER NOT NULL DEFAULT 1,"
        "  role INTEGER NOT NULL CHECK(role IN (0, 1, 2, 3))" // reference enums in database.h.
        ");", 

        // logs table
        "CREATE TABLE IF NOT EXISTS logs ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id INTEGER,"
        "  action TEXT NOT NULL,"
        "  ip_address TEXT,"
        "  details TEXT,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (user_id) REFERENCES accounts(id) ON DELETE SET NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_logs_user_id ON logs(user_id);"
        "CREATE INDEX IF NOT EXISTS idx_logs_created_at ON logs(created_at);"
    };

    sqlite3 *database = NULL;
    int err_msg = 0;

    if ((err_msg = sqlite3_open(db_file, &database)) != SQLITE_OK) {
        printf("Couldn't open database file and/or initialize the sqlite3 object in memory: %s", sqlite3_errmsg(err_msg));
        sqlite3_close(db_file);
        return 0;
    }
    
    // for every table set for our sql database statically defined, on initialization attempt to read an existing structure or create.
    size_t tables = sizeof(init_tables) / sizeof(char *);
    for (int t = 0; t < tables; t++)
        if ((sqlite3_exec(database, init_tables[t], NULL, NULL, &err_msg)) != SQLITE_OK) {
            printf("Failed to initialize our innately required tables for database.\nError: %s\n", sqlite3_errmsg(err_msg));
            sqlite3_close(db_file);
            return 0;
        }

    our_db.db_handler = database;
    return 1;
}