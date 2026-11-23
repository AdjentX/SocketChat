#include "headers/database.h"

Database our_db;

// enum, str, int
static const _role_map_t roles_arr[] = {
    { ROLE_ADMIN, "ADMIN", -1 },
    { ROLE_VIP, "VIP", 2 },
    { ROLE_REGULAR, "STANDARD", 1 },
    { ROLE_HOME, "HOME", 5 }
};

char *get_user_pass(char *userAlias) {
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT password FROM accounts WHERE username = ?;";
    if (sqlite3_prepare_v2(our_db.db_handler, (const char *)sql, -1, &stmt, NULL) != SQLITE_OK) 
        fprintf("Failed to prepare statement for password lookup!")
}

int user_manager(UserOperationType opt_type, void *args) {
    sqlite3_stmt *stmt = NULL;
    int result;
    char *sql;

    switch (opt_type) {
        case LOGIN:
            ClientInfo *login_user = (ClientInfo *)args;
            // login query prepared statement.
            sql = "SELECT id, password, username, max_conns, role FROM accounts WHERE username = ?;";
            if (sqlite3_prepare_v2(our_db.db_handler, (const char *)sql, -1, &stmt, NULL) != SQLITE_OK)
                fprintf(stderr, "Error: %s\n", sqlite3_errmsg(our_db.db_handler));

            sqlite3_bind_text(stmt, 1, login_user->username, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const char *stored_pass = (const char *)sqlite3_column_text(stmt, 1);
                if (stored_pass && !strcmp(stored_pass, login_user->password)) {
                    login_user->logged_in = true;
                    login_user->acc_info.userID = sqlite_column_int(stmt, 0);
                    login_user->acc_info.maxConns = sqlite3_column_int(stmt, 3);
                    login_user->acc_info.userRole = (role_t)sqlite_colun_int(stmt, 4);

                    sqlite3_finalize(stmt);
                    return 0;
                } else
                    sqlite3_finalize(stmt);
            } else
                sqlite3_finalize(stmt);
        break;

        case ADD_USER:
            char **new_user = (char **)args;

            char *user = new_user[0];
            char *pass = new_user[1];
            role_t role = atoi(new_user[3]);
            int max_conns = atoi(new_user[2]);

            if (strlen(new_user) < 4)
                fprintf(stderr, "ERROR: database.c -> user_manager(AddUser) - missing parameters!\n");

            // add user prepared statement.
            sql = "INSERT INTO  accounts (username, password, max_conns, role) VALUES (?, ?, ?, ?);";
            if (sqlite3_prepare_v2(our_db.db_handler, (const char *)sql, -1, &stmt, NULL) != SQLITE_OK)
                fprintf(stderr, "Error: %s\n", sqlite3_errmsg(our_db.db_handler));

            sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, max_conns);
            sqlite3_bind_int(stmt, 4, role);

            result = sqlite3_step(stmt);
            if (result = SQLITE_DONE) {
                sqlite3_finalize(stmt);
                return 0;
            } else if (result == SQLITE_CONSTRAINT)
                fprintf(stderr, "Error adding user: %s\n", sqlite3_errmsg(our_db.db_handler));
            else  
                fprintf(stderr, "ERROR: database.c -> user_manager(AddUser) - failure: %s", sqlite3_errmsg(our_db.db_handler));
            sqlite3_finalize(stmt);
        break;

        case DELETE_USER:
            char **del_user = (char **)args;
            
            if (strlen(del_user) <= 0)
                fprintf(stderr, "ERROR: database.c -> user_manager(DELETE_USER) requires the userID!\n");

            int user_id = atoi(del_user[0]);
            sql = "DELETE FROM accounts WHERE id = ?";

            // rest of the logic to delete acc, had to go to slave job...
            if (sqlite3_prepare_v2(our_db.db_handler, sql, -1, &stmt, NULL) != SQLITE_OK)
                fprintf(stderr, "ERROR: database.c -> user_manager(DELETE_USER), failed to prepare sql statement.\nReason: %s\n", sqlite3_error(our_db.db_handler));

            sqlite3_bind_int(stmt, 1, user_id);
            int deletion_attempt = sqlite3_step(stmt);
            if (deletion_attempt == SQLITE_ROW) {
                fprintf(stdout, "EVENT: database.c -> user_manager(DELETE_USER), user successfully deleted\nuser-alias: %s,\nuser-id: %d\n", (char*)sqlite3_column_text(stmt, 0), user_id); 
                sqlite3_finalize(stmt);
                return 0;
            } else if (deletion_attempt == SQLITE_DONE)
                fprintf(stderr, "database.c -> delete_user(), Execution failed: %s\n", sqlite3_errmsg(our_db.db_handler));
            else
                fprintf(stdout, "database.c -> delete_user(), user not found by submitted ID %d\n", user_id);
            
            sqlite3_finalize(stmt);

        case UPDATE_USER:
            char **new_pass = (char **)args;
            sql = "UPDATE accounts SET password = ?, role = ?, max_conns = ? WHERE id = ?;"; 

            if (strlen(new_pass) <= 0 || strlen(new_pass) > 4)
                fprintf(stderr, "ERROR: database.c -> manager_user(UPDATE_USER), too many or too few arguments passed.");   

            if (sqlite3_prepare_v2(our_db.db_handler, sql, -1, &stmt, NULL) != SQLITE_OK)
                fprintf(stderr, "ERROR: database.c -> user_manager(CHANGE_PASS), failed to update user.\nReason: %s!\n", sqlite3_errmsg(our_db.db_handler));
            
            char *updated_pass = (!new_pass[0]) ?

            sqlite_bind_text(stmt, 0, );
    }  
    return -1;
}

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