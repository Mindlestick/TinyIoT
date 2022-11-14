#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include <ctype.h>
#include <time.h>

#include "onem2m.h"

int main() {
    
    CIN cin1,cin2;

    //input example
    cin1.pi = "3-20220513091700249586";
    cin1.ri = "4-20220513T093154";
    cin1.ty = 4;
    cin1.ct = "20220513T093154";
    cin1.st = 1;
    cin1.rn = "cin1";
    cin1.lt = "20220513T093154";
    cin1.et = "20220513T093154";
    cin1.cs = 2;
    cin1.con = "ON";
    

    cin2.pi = "3-10220513091700249586";
    cin2.ri = "4-20220808T113154";
    cin2.ty = 4;
    cin2.ct = "20220808T113154";
    cin2.st = 1;
    cin2.rn = "cin2";
    cin2.lt = "20220808T113154";
    cin2.et = "20220808T113154";
    cin2.cs = 2;
    cin2.con = "OFF";
    

    // [success -> 1] 
    if(DB_Store_CIN(&cin1)) fprintf(stderr, "store success!\n");
    if(DB_Store_CIN(&cin2)) fprintf(stderr, "store success!\n");

    char* DATABASE = "RESOURCE.db";
    DB_display(DATABASE);

    return 0;
}

/*DB CREATE*/
DB* DB_CREATE_(DB *dbp){
    int ret;

    ret = db_create(&dbp, NULL, 0);
    if (ret) {
        fprintf(stderr, "db_create : %s\n", db_strerror(ret));
        fprintf(stderr, "File ERROR\n");
        exit(0);
    }
    return dbp;
}

/*DB Open*/
DB* DB_OPEN_(DB *dbp,char* DATABASE){
    int ret;

    ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE);
        fprintf(stderr, "DB Open ERROR\n");
        exit(0);
    }
    return dbp;
}

/*DB Get Cursor*/
DBC* DB_GET_CURSOR(DB *dbp, DBC *dbcp){
    int ret;
    
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        fprintf(stderr, "Cursor ERROR");
        exit(0);
    }
    return dbcp;
}

int DB_Store_CIN(CIN *cin_object) {
    char* DATABASE = "RESOURCE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    
    // if input == NULL
    if (cin_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (cin_object->rn == NULL) cin_object->rn = " ";
    if (cin_object->pi == NULL) cin_object->pi = "NULL";
    if (cin_object->ty == '\0') cin_object->ty = 0;
    if (cin_object->ct == NULL) cin_object->ct = " ";
    if (cin_object->lt == NULL) cin_object->lt = " ";
    if (cin_object->et == NULL) cin_object->et = " ";

    if (cin_object->con == NULL) cin_object->con = " ";
    if (cin_object->cs == '\0') cin_object->cs = 0;
    if (cin_object->st == '\0') cin_object->st = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cin_object->ri;
    key_ri.size = strlen(cin_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%d;%d",
            cin_object->rn,cin_object->pi,cin_object->ty,cin_object->ct,cin_object->lt,cin_object->et,
            cin_object->con,cin_object->cs,cin_object->st);

    data.data = str;
    data.size = strlen(str) + 1;

    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_ri, &data, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    /* DB close */
    dbcp->close(dbcp);
    dbp->close(dbp, 0); 

    return 1;
}

int DB_display(char* database)
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
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ){
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