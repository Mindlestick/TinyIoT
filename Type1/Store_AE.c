#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int display(char* database);

int main() {
    AE ae1;
    AE ae2;
    AE ae3;
    char* DATABASE = "AE.db";

    //input sample
    ae1.rn = "Sensor1";
    ae1.ty = 2;
    ae1.pi = "5-20191210093452845";
    ae1.ri = "TAE1";
    ae1.ct = "20220513T083900";
    ae1.lt = "20220513T083900";
    ae1.et = "20240513T083900";
    ae1.api = "tinyProject1";
    ae1.rr = true;
    ae1.aei = "TAE1";

    ae2.rn = "Sensor2";
    ae2.ty = 2;
    ae2.pi = "5-20191210093452845";
    ae2.ri = "TAE2";
    ae2.ct = "20220513T083900";
    ae2.lt = "20220513T083900";
    ae2.et = "20240513T083900";
    ae2.api = "tinyProject2";
    ae2.rr = true;
    ae2.aei = "TAE2";

    ae3.rn = "Sensor3";
    ae3.ty = 2;
    ae3.pi = "5-20191210093452845";
    ae3.ri = "TAE3";
    ae3.ct = "20220513T083900";
    ae3.lt = "20220513T083900";
    ae3.et = "20240513T083900";
    ae3.api = "tinyProject3";
    ae3.rr = true;
    ae3.aei = "TAE3";


    // [success -> 1] 
    if (Store_AE(&ae1)) fprintf(stderr, "store success!\n");
    if (Store_AE(&ae3)) fprintf(stderr, "store success!\n");
    if (Store_AE(&ae2)) fprintf(stderr, "store success!\n");


    // print
    display(DATABASE);

    return 0;
}

int Store_AE(AE* ae_object) {
    char* DATABASE = "AE.db";
    DB* dbp;    // db handle
    DBC* dbcp;
    FILE* error_file_pointer;
    DBT key_ct, key_lt, key_rn, key_ri, key_pi, key_ty, key_et, key_api, key_rr,key_aei;
    DBT data_ct, data_lt, data_rn, data_ri, data_pi, data_ty, data_et, data_api, data_rr, data_aei;  // storving key and real data
    int ret;        // template value

    char* program_name = "my_prog";

    // if input == NULL
    if (ae_object->ri == NULL) ae_object->ri = "";
    if (ae_object->rn == NULL) ae_object->rn = "";
    if (ae_object->pi == NULL) ae_object->pi = "";
    if (ae_object->ty == '\0') ae_object->ty = -1;
    if (ae_object->ct == NULL) ae_object->ct = "";
    if (ae_object->lt == NULL) ae_object->lt = "";
    if (ae_object->et == NULL) ae_object->et = "";

    if (ae_object->rr == '\0') ae_object->rr = true;
    if (ae_object->api == NULL) ae_object->api = "";
    if (ae_object->aei == NULL) ae_object->aei = "";

    ret = db_create(&dbp, NULL, 0);
    if (ret) {
        fprintf(stderr, "db_create : %s\n", db_strerror(ret));
        fprintf(stderr, "File ERROR\n");
        exit(1);
    }

    dbp->set_errfile(dbp, error_file_pointer);
    dbp->set_errpfx(dbp, program_name);

    /*Set duplicate*/
    ret = dbp->set_flags(dbp, DB_DUP);
    if (ret != 0) {
        dbp->err(dbp, ret, "Attempt to set DUPSORT flag failed.");
        fprintf(stderr, "Flag Set ERROR\n");
        dbp->close(dbp, 0);
        return(ret);
    }

    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE);
        fprintf(stderr, "DB Open ERROR\n");
        return 0;
    }
    
    /*
* The DB handle for a Btree database supporting duplicate data
* items is the argument; acquire a cursor for the database.
*/
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        fprintf(stderr, "Cursor ERROR");
        return 0;
    }

    /* keyand data must initialize */
    memset(&key_rn, 0, sizeof(DBT));
    memset(&key_ri, 0, sizeof(DBT));
    memset(&key_pi, 0, sizeof(DBT));
    memset(&key_ty, 0, sizeof(DBT));
    memset(&key_ct, 0, sizeof(DBT));
    memset(&key_lt, 0, sizeof(DBT));
    memset(&key_et, 0, sizeof(DBT));
    memset(&key_api, 0, sizeof(DBT));
    memset(&key_rr, 0, sizeof(DBT));
    memset(&key_aei, 0, sizeof(DBT));

    memset(&data_rn, 0, sizeof(DBT));
    memset(&data_ri, 0, sizeof(DBT));
    memset(&data_pi, 0, sizeof(DBT));
    memset(&data_ty, 0, sizeof(DBT));
    memset(&data_ct, 0, sizeof(DBT));
    memset(&data_lt, 0, sizeof(DBT));
    memset(&data_et, 0, sizeof(DBT));
    memset(&data_api, 0, sizeof(DBT));
    memset(&data_rr, 0, sizeof(DBT));
    memset(&data_aei, 0, sizeof(DBT));
    
    // Store key & data
    data_rn.data = ae_object->rn;
    data_rn.size = strlen(ae_object->rn) + 1;
    key_rn.data = "rn";
    key_rn.size = strlen("rn") + 1;

    data_ri.data = ae_object->ri;
    data_ri.size = strlen(ae_object->ri) + 1;
    key_ri.data = "ri";
    key_ri.size = strlen("ri") + 1;

    data_pi.data = ae_object->pi;
    data_pi.size = strlen(ae_object->pi) + 1;
    key_pi.data = "pi";
    key_pi.size = strlen("pi") + 1;

    data_ty.data = &ae_object->ty;
    data_ty.size = sizeof(ae_object->ty);
    key_ty.data = "ty";
    key_ty.size = strlen("ty") + 1;

    data_ct.data = ae_object->ct;
    data_ct.size = strlen(ae_object->ct) + 1;
    key_ct.data = "ct";
    key_ct.size = strlen("ct") + 1;

    data_lt.data = ae_object->lt;
    data_lt.size = strlen(ae_object->lt) + 1;
    key_lt.data = "lt";
    key_lt.size = strlen("lt") + 1;

    data_et.data = ae_object->et;
    data_et.size = strlen(ae_object->et) + 1;
    key_et.data = "et";
    key_et.size = strlen("et") + 1;

    data_api.data = ae_object->api;
    data_api.size = strlen(ae_object->api) + 1;
    key_api.data = "api";
    key_api.size = strlen("api") + 1;

    data_rr.data = &ae_object->rr;
    data_rr.size = sizeof(ae_object->rr);
    key_rr.data = "rr";
    key_rr.size = strlen("rr") + 1;

    data_aei.data = ae_object->aei;
    data_aei.size = strlen(ae_object->aei) + 1;
    key_aei.data = "aei";
    key_aei.size = strlen("aei") + 1;
 

    if ((ret = dbcp->put(dbcp, &key_ri, &data_ri, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_rn, &data_rn, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_pi, &data_pi, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_ty, &data_ty, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_ct, &data_ct, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_lt, &data_lt, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_et, &data_et, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_api, &data_api, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_rr, &data_rr, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");
    if ((ret = dbcp->put(dbcp, &key_aei, &data_aei, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");


    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close

    return 1;
}

int display(char* database)
{
    printf("[Display] %s \n", database); //DB name print

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int close_db, close_dbc, ret;

    close_db = close_dbc = 0;

    /* Open the database. */
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr,
            "%s: db_create: %s\n", database, db_strerror(ret));
        return (1);
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

    /* Walk through the database and print out the key/data pairs. */
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        //int
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ) {
            fprintf(stderr, "%.*s : %d\n", (int)key.size, (char*)key.data, *(int*)data.data);
        }
        //bool
        else if (strncmp(key.data, "rr", key.size) == 0) {
            fprintf(stderr, "%.*s : ", (int)key.size, (char*)key.data);
            if (*(bool*)data.data == true)
                fprintf(stderr, "true\n");
            else
                fprintf(stderr, "false\n");
        }

        //string
        else {
            printf("%.*s : %.*s\n",
                (int)key.size, (char*)key.data,
                (int)data.size, (char*)data.data);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

    //DB close
    dbcp->close(dbcp);
    dbp->close(dbp, 0);

err:    if (close_dbc && (ret = dbcp->close(dbcp)) != 0)
dbp->err(dbp, ret, "DBcursor->close");
if (close_db && (ret = dbp->close(dbp, 0)) != 0)
fprintf(stderr,
    "%s: DB->close: %s\n", database, db_strerror(ret));
return (0);
}