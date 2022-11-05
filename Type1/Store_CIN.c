#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int display(char* database);

int main() {
    CIN cin1;
    CIN cin2;
    CIN cin3;
    CIN cin4;
    CIN cin5;

    //input example
    cin1.pi = "3-20220513091700249586";
    cin1.ri = "4-20220513T093154";
    cin1.ty = 4;
    cin1.ct = "20220513T093154";
    cin1.st = 1;
    cin1.rn = "4-20220513T093154";
    cin1.lt = "20220513T093154";
    cin1.et = "20220513T093154";
    cin1.cs = 2;
    cin1.con = "ON";
    cin1.csi = "test";

    cin2.pi = "3-20210513091700249586";
    cin2.ri = "4-20220808T113154";
    cin2.ty = 4;
    cin2.ct = "20220808T113154";
    cin2.st = 1;
    cin2.rn = "4-20220808T113154";
    cin2.lt = "20220808T113154";
    cin2.et = "20220808T113154";
    cin2.cs = 2;
    cin2.con = "OFF";

    cin3.pi = "3-20220513091700249586";
    cin3.ri = "4-20220809T093154";
    cin3.ty = 4;
    cin3.ct = "20220809T093154";
    cin3.st = 1;
    cin3.rn = "4-20220808T093154";
    cin3.lt = "20220808T093154";
    cin3.et = "20220808T093154";
    cin3.cs = 2;
    cin3.con = "ON";

    cin4.pi = "3-20220513091700249586";
    cin4.ri = "4-20220807T233154";
    cin4.ty = 4;
    cin4.ct = "20220807T233154";
    cin4.st = 1;
    cin4.rn = "4-20220807T233154";
    cin4.lt = "20220807T233154";
    cin4.et = "20220807T233154";
    cin4.cs = 2;
    cin4.con = "ON";

    cin5.pi = "3-20220513091700249586";
    cin5.ri = "4-20220807T113154";
    cin5.ty = 4;
    cin5.ct = "20220807T113154";
    cin5.st = 1;
    cin5.rn = "4-20220807T113154";
    cin5.lt = "20220807T113154";
    cin5.et = "20220807T113154";
    cin5.cs = 2;
    cin5.con = "ON";

    // [success -> 1] 
    if (Store_CIN(&cin1)) printf("store success!\n");
    if (Store_CIN(&cin2)) printf("store success!\n");
    if (Store_CIN(&cin3)) printf("store success!\n");
    if (Store_CIN(&cin4)) printf("store success!\n");
    if (Store_CIN(&cin5)) printf("store success!\n");

    // print
    char* DATABASE = "CIN.db";
    display(DATABASE);

    return 0;
}

int Store_CIN(CIN *cin_object)
{
    char* DATABASE = "CIN.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    FILE* error_file_pointer;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    DBT key_ct, key_lt, key_rn, key_ri, key_pi, key_ty, key_et, key_st, key_cs, key_con, key_csi;
    DBT data_ct, data_lt, data_rn, data_ri, data_pi, data_ty, data_et, data_st, data_cs, data_con, data_csi;  // storving key and real data

    char* program_name = "my_prog";

    // if input == NULL
    if (cin_object->ri == NULL) cin_object->ri = "";
    if (cin_object->rn == NULL) cin_object->rn = "";
    if (cin_object->pi == NULL) cin_object->pi = "";
    if (cin_object->ty == '\0') cin_object->ty = -1;
    if (cin_object->ct == NULL) cin_object->ct = "";
    if (cin_object->lt == NULL) cin_object->lt = "";
    if (cin_object->et == NULL) cin_object->et = "";

    if (cin_object->con == NULL) cin_object->con = "";
    if (cin_object->csi == NULL) cin_object->csi = "";
    if (cin_object->cs == '\0') cin_object->cs = -1;
    if (cin_object->st == '\0') cin_object->st = -1;
    
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
 
    // key and data must initialize
    memset(&key_ct, 0, sizeof(DBT));
    memset(&key_lt, 0, sizeof(DBT));
    memset(&key_rn, 0, sizeof(DBT));
    memset(&key_ri, 0, sizeof(DBT));
    memset(&key_pi, 0, sizeof(DBT));
    memset(&key_ty, 0, sizeof(DBT));
    memset(&key_et, 0, sizeof(DBT));
    memset(&key_cs, 0, sizeof(DBT));
    memset(&key_con, 0, sizeof(DBT));
    memset(&key_st, 0, sizeof(DBT));
    memset(&key_csi, 0, sizeof(DBT));

    memset(&data_ct, 0, sizeof(DBT));
    memset(&data_lt, 0, sizeof(DBT));
    memset(&data_rn, 0, sizeof(DBT));
    memset(&data_ri, 0, sizeof(DBT));
    memset(&data_pi, 0, sizeof(DBT));
    memset(&data_ty, 0, sizeof(DBT));
    memset(&data_et, 0, sizeof(DBT));
    memset(&data_cs, 0, sizeof(DBT));
    memset(&data_con, 0, sizeof(DBT));
    memset(&data_st, 0, sizeof(DBT));
    memset(&data_csi, 0, sizeof(DBT));


    /* initialize the data to be the first of two duplicate records. */
    data_rn.data = cin_object->rn;
    data_rn.size = strlen(cin_object->rn) + 1;
    key_rn.data = "rn";
    key_rn.size = strlen("rn") + 1;

    data_ri.data = cin_object->ri;
    data_ri.size = strlen(cin_object->ri) + 1;
    key_ri.data = "ri";
    key_ri.size = strlen("ri") + 1;

    data_pi.data = cin_object->pi;
    data_pi.size = strlen(cin_object->pi) + 1;
    key_pi.data = "pi";
    key_pi.size = strlen("pi") + 1;

    data_ct.data = cin_object->ct;
    data_ct.size = strlen(cin_object->ct) + 1;
    key_ct.data = "ct";
    key_ct.size = strlen("ct") + 1;

    data_lt.data = cin_object->lt;
    data_lt.size = strlen(cin_object->lt) + 1;
    key_lt.data = "lt";
    key_lt.size = strlen("lt") + 1;

    data_et.data = cin_object->et;
    data_et.size = strlen(cin_object->et) + 1;
    key_et.data = "et";
    key_et.size = strlen("et") + 1;

    data_con.data = cin_object->con;
    data_con.size = strlen(cin_object->con) + 1;
    key_con.data = "con";
    key_con.size = strlen("con") + 1;

    data_csi.data = cin_object->csi;
    data_csi.size = strlen(cin_object->csi) + 1;
    key_csi.data = "csi";
    key_csi.size = strlen("csi") + 1;

    data_ty.data = &cin_object->ty;
    data_ty.size = sizeof(cin_object->ty);
    key_ty.data = "ty";
    key_ty.size = strlen("ty") + 1;

    data_st.data = &cin_object->st;
    data_st.size = sizeof(cin_object->st);
    key_st.data = "st";
    key_st.size = strlen("st") + 1;

    data_cs.data = &cin_object->cs;
    data_cs.size = sizeof(cin_object->cs);
    key_cs.data = "cs";
    key_cs.size = strlen("cs") + 1;

    /* CIN -> only one & first */
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
    if ((ret = dbcp->put(dbcp, &key_st, &data_st, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_et, &data_et, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    if ((ret = dbcp->put(dbcp, &key_cs, &data_cs, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_con, &data_con, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");
    if ((ret = dbcp->put(dbcp, &key_csi, &data_csi, DB_KEYLAST)) != 0)
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