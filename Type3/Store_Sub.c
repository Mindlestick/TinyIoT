#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"
#define DB_STR_MAX 2048
#define DB_SEP ";"

int main() {
    Sub sub1;
    Sub sub2;
    Sub sub3;

    sub1.rn = "sub1";
    sub1.ri = "23-2022040684653299304";
    sub1.pi = "3-20220406084023203796";
    sub1.nu = "http://223.131.176.101:3000/ct=json";
    sub1.net = "1";
    sub1.ct = "20220406T084653";
    sub1.et = "20220406T084653";
    sub1.lt = "20220406T084653";
    sub1.ty = 23;
    sub1.nct = 1;

    sub2.rn = "sub2";
    sub2.ri = "23-2021040684653299304";
    sub2.pi = "3-20220406084023203796";
    sub2.nu = "http://223.131.176.101:3000/ct=json";
    sub2.net = "2";
    sub2.ct = "20210406T084653";
    sub2.et = "20210406T084653";
    sub2.lt = "20210406T084653";
    sub2.ty = 23;
    sub2.nct = 1;

    sub3.rn = "sub3";
    sub3.ri = "23-2023040684653299304";
    sub3.pi = "3-20220406084023203796";
    sub3.nu = "http://223.131.176.101:3000/ct=json";
    sub3.net = "15";
    sub3.ct = "20210406T084653";
    sub3.et = "20210406T084653";
    sub3.lt = "20210406T084653";
    sub3.ty = 23;
    sub3.nct = 1;

    DB_Store_Sub(&sub1);
    DB_Store_Sub(&sub2);
    DB_Store_Sub(&sub3);        
    DB_display("Sub.db");

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
        return NULL;
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


int DB_Store_Sub(Sub *sub_object) {
    char* DATABASE = "Sub.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value
    DBT key_ri;
    DBT data;  // storving key and real data

    // if input == NULL
    if (sub_object->ri == NULL){
        fprintf(stderr, "The ri is NULL\n");
        return -1;
    }
    if (sub_object->rn == NULL) sub_object->rn = "";
    if (sub_object->pi == NULL) {
        fprintf(stderr, "The pi is NULL\n");
        return -1;
    }
    if (sub_object->nu == NULL) sub_object->nu = "";
    if (sub_object->net == NULL) sub_object->net = "1";
    
    if (sub_object->ct == NULL) sub_object->ct = "";
    if (sub_object->et == NULL) sub_object->et = "";
    if (sub_object->lt == NULL) sub_object->lt = "";
    if (sub_object->ty == '\0') sub_object->ty = 23;
    if (sub_object->nct == '\0') sub_object->nct = 1;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = sub_object->ri;
    key_ri.size = strlen(sub_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%s;%s;%d;%s;%s;%s;%d",
            sub_object->rn,sub_object->pi,sub_object->nu,sub_object->net,
            sub_object->ty,sub_object->ct,sub_object->lt,sub_object->et,
            sub_object->nct);

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
            strncmp(key.data, "cs", key.size) == 0 ||
            strncmp(key.data, "nct", key.size) == 0
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