#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {
    CSE *cse = Get_CSE();
    fprintf(stderr, "%s\n%s\n%s\n%s\n%s\n%s\n%d\n", 
        cse->ri,cse->rn, cse->pi, cse->csi, cse->ct, cse->lt, cse->ty);
    
    return 0;
}

CSE* Get_CSE() {
    fprintf(stderr, "[Get CSE]\n");

    //store CSE Object
    CSE* new_cse = (CSE*)malloc(sizeof(CSE));

    char* database = "CSE.db";
    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int close_db, close_dbc, ret;

    close_db = close_dbc = 0;

    /* Open the database. */
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr,
            "%s: db_create: %s\n", database, db_strerror(ret));
        return 0;
    }
    close_db = 1;

    /* Turn on additional error output. */
    dbp->set_errfile(dbp, stderr);
    dbp->set_errpfx(dbp, database);

    /* Open the database. */
    if ((ret = dbp->open(dbp, NULL, database, NULL,
        DB_UNKNOWN, DB_RDONLY, 0)) != 0) {
        dbp->err(dbp, ret, "%s: DB->open", database);
        goto err;
    }

    /* Acquire a cursor for the database. */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        goto err;
    }
    close_dbc = 1;

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    //store CSE
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {

        if (strncmp(key.data, "ri", key.size) == 0) {
            new_cse->ri = malloc(data.size);
            strcpy(new_cse->ri, data.data);
        }
        if (strncmp(key.data, "pi", key.size) == 0) {
            new_cse->pi = malloc(data.size);
            strcpy(new_cse->pi, data.data);
        }
        if (strncmp(key.data, "rn", key.size) == 0) {
            new_cse->rn = malloc(data.size);
            strcpy(new_cse->rn, data.data);
        }
        if (strncmp(key.data, "ty", key.size) == 0) {
            new_cse->ty = *(int*)data.data;
        }
        if (strncmp(key.data, "csi", key.size) == 0) {
            new_cse->csi = malloc(data.size);
            strcpy(new_cse->csi, data.data);
        }
        if (strncmp(key.data, "lt", key.size) == 0) {
            new_cse->lt = malloc(data.size);
            strcpy(new_cse->lt, data.data);
        }
        if (strncmp(key.data, "ct", key.size) == 0) {
            new_cse->ct = malloc(data.size);
            strcpy(new_cse->ct, data.data);
        }
    }

    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        exit(0);
    }

err:    if (close_dbc && (ret = dbcp->close(dbcp)) != 0)
dbp->err(dbp, ret, "DBcursor->close");
if (close_db && (ret = dbp->close(dbp, 0)) != 0)
fprintf(stderr,
    "%s: DB->close: %s\n", database, db_strerror(ret));
return new_cse;
}
