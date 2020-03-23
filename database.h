#ifndef RPG_DATABASE
#define RPG_DATABASE

#include <sqlite3.h>

#define DB_PATH "assets/assets.db"

/* ************************************
 * The next macros are defined to reduce code complexity when managing sqlite DB
 * - all DB code should be surrounded by INIT_DB and CLOSE_DB
 * - MODIFY_DB can be used on its own
 * - queries should be done with QUERY_DB and closed by END_QUERY
 * - Each query values can be extracted with GET_QUERY macros, using i for loop
 *   variable name
 * ************************************ */

#define INIT_DB \
    sqlite3* db; sqlite3_stmt *stmt; \
    int rc; \
    TRY \
        if (sqlite3_open(DB_PATH, &db)) \
            THROW(DATABASE_READ_FAILURE);\
    CATCH_HANDLE(DATABASE_READ_FAILURE, NULL) \
    ETRY;

#define MODIFY_DB(query) \
    TRY \
    { \
        _RC_PREPARE(query, QUERY_MODIFY_FAILURE) \
        _RC_STEP_FUNC; \
        _RC_STEP_CHECK \
    } \
    CATCH_HANDLE(QUERY_MODIFY_FAILURE, sqlite3_errmsg(db)) \
    CATCH_HANDLE(QUERY_END_FAILURE, sqlite3_errmsg(db)) \
    ETRY;

#define QUERY_DB(query) \
    TRY \
    { \
        _RC_PREPARE(query, QUERY_READ_FAILURE) \
        while ((_RC_STEP_FUNC) == SQLITE_ROW) {

#define END_QUERY \
        } \
        _RC_STEP_CHECK \
    } \
    CATCH_HANDLE(QUERY_READ_FAILURE, sqlite3_errmsg(db)) \
    CATCH_HANDLE(QUERY_END_FAILURE, sqlite3_errmsg(db)) \
    ETRY;

#define CLOSE_DB \
    sqlite3_finalize(stmt); \
    sqlite3_close(db);

#define GET_QUERY_INT(i) sqlite3_column_int(stmt, i)
#define GET_QUERY_TXT(i) sqlite3_column_text(stmt, i)

/* ************************************
 * These macros are for internal use
 * ************************************ */

#define _RC_PREPARE(query, e) \
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL); \
        if (rc != SQLITE_OK) \
            THROW(e);

#define _RC_STEP_FUNC rc = sqlite3_step(stmt)
#define _RC_STEP_CHECK \
        if (rc != SQLITE_DONE) \
            THROW(QUERY_END_FAILURE); \

#endif // define RPG_DATABASE
