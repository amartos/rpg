#ifndef RPG_DATABASE
#define RPG_DATABASE

#define DB_PATH "ignore/assets.db"

#define INIT_DB \
    sqlite3* db; sqlite3_stmt *stmt; \
    int rc; \
    TRY \
    { \
        if (sqlite3_open(DB_PATH, &db)) \
            THROW(DATABASE_READ_FAILURE);\
    } \
    CATCH(DATABASE_READ_FAILURE) \
    { \
        printf("database read failure\n"); \
    } \
    ETRY;

#define QUERY_DB(query) \
    TRY \
    { \
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL); \
        if (rc != SQLITE_OK) \
            THROW(QUERY_READ_FAILURE); \
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

#define END_QUERY \
        } \
        if (rc != SQLITE_DONE) \
            THROW(QUERY_END_FAILURE); \
    } \
    CATCH(QUERY_READ_FAILURE) \
    { \
        printf("query read failure\n"); \
    } \
    CATCH(QUERY_END_FAILURE) \
    { \
        printf("query end failure\n"); \
    } \
    ETRY;

#define CLOSE_DB \
    sqlite3_finalize(stmt); \
    sqlite3_close(db);

#define GET_QUERY_INT(i) sqlite3_column_int(stmt, i)
#define GET_QUERY_TXT(i) sqlite3_column_text(stmt, i)

#endif // define RPG_DATABASE
