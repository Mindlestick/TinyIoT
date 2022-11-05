#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int display(char* database);

int main() {
    CNT cnt1;
    CNT cnt2;
    CNT cnt3;

    //input example
    cnt1.pi = "TAE1";
    cnt1.ri = "3-20220513093154147745";
    cnt1.ty = 3;
    cnt1.ct = "202205T093154";
    cnt1.st = 0;
    cnt1.rn = "status1";
    cnt1.lt = "20220513T093154";
    cnt1.et = "20220513T093154";
    cnt1.cni = 0;
    cnt1.cbs = 0;

    cnt2.pi = "TAE1";
    cnt2.ri = "3-20210513093154147745";
    cnt2.ty = 3;
    cnt2.ct = "202105T093154";
    cnt2.st = 0;
    cnt2.rn = "status2";
    cnt2.lt = "20210513T093154";
    cnt2.et = "20210513T093154";
    cnt2.cni = 0;
    cnt2.cbs = 0;

    cnt3.pi = "TAE2";
    cnt3.ri = "3-20200513093154147745";
    cnt3.ty = 3;
    cnt3.ct = "202005T093154";
    cnt3.st = 0;
    cnt3.rn = "status3";
    cnt3.lt = "20200513T093154";
    cnt3.et = "20200513T093154";
    cnt3.cni = 0;
    cnt3.cbs = 0;


    // [success -> 1] 
    if (Store_CNT(&cnt1)) printf("store success!\n");
    if (Store_CNT(&cnt2)) printf("store success!\n");
    if (Store_CNT(&cnt3)) printf("store success!\n");

    // print
    char* DATABASE = "CNT.db";
    display(DATABASE);

    return 0;
}

int Store_CNT(CNT *cnt_object)
{
    char* DATABASE = "CNT.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    FILE* error_file_pointer;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    DBT key_ct, key_lt, key_rn, key_ri, key_pi, key_ty, key_et, key_cni, key_cbs, key_st;
    DBT data_ct, data_lt, data_rn, data_ri, data_pi, data_ty, data_et, data_cni, data_cbs, data_st;  // storving key and real data

    char* program_name = "my_prog";

    // if input == NULL
    if (cnt_object->ri == NULL) cnt_object->ri = "";
    if (cnt_object->rn == NULL) cnt_object->rn = "";
    if (cnt_object->pi == NULL) cnt_object->pi = "";
    if (cnt_object->ty == '\0') cnt_object->ty = -1;
    if (cnt_object->ct == NULL) cnt_object->ct = "";
    if (cnt_object->lt == NULL) cnt_object->lt = "";
    if (cnt_object->et == NULL) cnt_object->et = "";

    if (cnt_object->cni == '\0') cnt_object->cni = -1;
    if (cnt_object->cbs == '\0') cnt_object->cbs = -1;
    if (cnt_object->st == '\0') cnt_object->st = -1;

    ret = db_create(&dbp, NULL, 0);
    if (ret) {
        fprintf(stderr, "db_create : %s\n", db_strerror(ret));
        printf("File ERROR\n");
        exit(1);
    }

    dbp->set_errfile(dbp, error_file_pointer);
    dbp->set_errpfx(dbp, program_name);

    /*Set duplicate*/
    ret = dbp->set_flags(dbp, DB_DUP);
    if (ret != 0) {
        dbp->err(dbp, ret, "Attempt to set DUPSORT flag failed.");
        printf("Flag Set ERROR\n");
        dbp->close(dbp, 0);
        return(ret);
    }

    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE);
        printf("DB Open ERROR\n");
        exit(1);
    }

    /*
  * The DB handle for a Btree database supporting duplicate data
  * items is the argument; acquire a cursor for the database.
  */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        printf("Cursor ERROR");
        exit(1);
    }
 
    /* keyand data must initialize */
    memset(&key_ct, 0, sizeof(DBT));
    memset(&key_lt, 0, sizeof(DBT));
    memset(&key_rn, 0, sizeof(DBT));
    memset(&key_ri, 0, sizeof(DBT));
    memset(&key_pi, 0, sizeof(DBT));
    memset(&key_ty, 0, sizeof(DBT));
    memset(&key_et, 0, sizeof(DBT));
    memset(&key_cni, 0, sizeof(DBT));
    memset(&key_cbs, 0, sizeof(DBT));
    memset(&key_st, 0, sizeof(DBT));

    memset(&data_ct, 0, sizeof(DBT));
    memset(&data_lt, 0, sizeof(DBT));
    memset(&data_rn, 0, sizeof(DBT));
    memset(&data_ri, 0, sizeof(DBT));
    memset(&data_pi, 0, sizeof(DBT));
    memset(&data_ty, 0, sizeof(DBT));
    memset(&data_et, 0, sizeof(DBT));
    memset(&data_cni, 0, sizeof(DBT));
    memset(&data_cbs, 0, sizeof(DBT));
    memset(&data_st, 0, sizeof(DBT));


    /* initialize the data to be the first of two duplicate records. */
    data_rn.data = cnt_object->rn;
    data_rn.size = strlen(cnt_object->rn) + 1;
    key_rn.data = "rn";
    key_rn.size = strlen("rn") + 1;

    data_ri.data = cnt_object->ri;
    data_ri.size = strlen(cnt_object->ri) + 1;
    key_ri.data = "ri";
    key_ri.size = strlen("ri") + 1;

    data_pi.data = cnt_object->pi;
    data_pi.size = strlen(cnt_object->pi) + 1;
    key_pi.data = "pi";
    key_pi.size = strlen("pi") + 1;

    data_ct.data = cnt_object->ct;
    data_ct.size = strlen(cnt_object->ct) + 1;
    key_ct.data = "ct";
    key_ct.size = strlen("ct") + 1;

    data_lt.data = cnt_object->lt;
    data_lt.size = strlen(cnt_object->lt) + 1;
    key_lt.data = "lt";
    key_lt.size = strlen("lt") + 1;

    data_et.data = cnt_object->et;
    data_et.size = strlen(cnt_object->et) + 1;
    key_et.data = "et";
    key_et.size = strlen("et") + 1;

    data_ty.data = &cnt_object->ty;
    data_ty.size = sizeof(cnt_object->ty);
    key_ty.data = "ty";
    key_ty.size = strlen("ty") + 1;

    data_st.data = &cnt_object->st;
    data_st.size = sizeof(cnt_object->st);
    key_st.data = "st";
    key_st.size = strlen("st") + 1;

    data_cni.data = &cnt_object->cni;
    data_cni.size = sizeof(cnt_object->cni);
    key_cni.data = "cni";
    key_cni.size = strlen("cni") + 1;

    data_cbs.data = &cnt_object->cbs;
    data_cbs.size = sizeof(cnt_object->cbs);
    key_cbs.data = "cbs";
    key_cbs.size = strlen("cbs") + 1;

    /* CNT -> only one & first */
    if ((ret = dbcp->put(dbcp, &key_ri, &data_ri, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_rn, &data_rn, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_pi, &data_pi, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_ty, &data_ty, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    if ((ret = dbcp->put(dbcp, &key_ct, &data_ct, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_lt, &data_lt, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_cni, &data_cni, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_st, &data_st, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_et, &data_et, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_cbs, &data_cbs, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

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
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0){
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ) {
            printf("%.*s : %d\n", (int)key.size, (char*)key.data, *(int*)data.data);
        }
        else if (strncmp(key.data, "rr", key.size) == 0) {
            printf("%.*s : ", (int)key.size, (char*)key.data);
            if (*(bool*)data.data == true)
                printf("true\n");
            else
                printf("false\n");
        }
        else {
            printf("%.*s : %.*s\n",
                (int)key.size, (char*)key.data,
                (int)data.size, (char*)data.data);
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
return (0);
}