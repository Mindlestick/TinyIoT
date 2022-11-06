#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {
    char* label1 = "label1";
    char* uri1 = "http://1";

    char* label2 = "label2";
    char* uri2 = "http://2";

    char* label3 = "myCNT";
    char* uri3 = "/TinyIoT/test_AE1/test_CNT1";

    Store_Label(label1, uri1);
    Store_Label(label2, uri2);
    Store_Label(label3, uri3);

    display("LABEL.db");

    char* return_uri = Label_To_URI(label2);
    char* return_uri2 = Label_To_URI("/TinyIoT/test_AE1/test_CNT1");
    char* return_label = URI_To_Label(uri3);

    printf("\nlabel2 -> %s\n", return_uri);
    if (return_uri2 == NULL) printf("/TinyIoT/test_AE1/test_CNT1 -> NULL\n");
    printf("/TinyIoT/test_AE1/test_CNT1 -> %s\n", return_label);

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
DB* DB_OPEN_(DB *dbp){
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


/*
 Function name: Store_Label
 Input: string label, string URI
 Return: 1 or -1
 Description:
 Function to store Label as Key and URI as value in LABEL.db
*/
int Store_Label(char* label, char* uri) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_label;
    DBT data_uri;  // storving key and real data

    // if input is NULL => return -1
    if (label == NULL) {
        fprintf(stderr, "Label is empty\n");
        return -1;
    }
    if (uri == NULL) {
        fprintf(stderr, "URI is empty\n");
        return -1;
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* keyand data must initialize */
    memset(&key_label, 0, sizeof(DBT));
    memset(&data_uri, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_label.data = label;
    key_label.size = strlen(label) + 1;
    data_uri.data = uri;
    data_uri.size = strlen(uri) + 1;

    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_label, &data_uri, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close

    return 1;
}

/*
 Function name: Label_To_URI
 Input: string label
 Return: string uri
 Description:
 It receives the Label value as an argument from the <key: Label, value:URI> type DB 
 and returns the URI.
*/
char* Label_To_URI(char* label) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    // if input is NULL => return -1
    if (label == NULL) {
        fprintf(stderr, "Label is empty\n");
        exit(-1);
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* 0 if LABEL does not exist, 1 if present */
    int flag = 0;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, label, key.size) == 0) {
            flag=1;
            return (char*)data.data;

            //DB close
            dbcp->close(dbcp);
            dbp->close(dbp, 0); 
            break;
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        exit(0);
    }
    if (flag == 0)
        return NULL;

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close
}

/*
 Function name: URI_To_Label
 Input: string URI
 Return: string label
 Description:
 It receives the URI value as an argument from the <key: Label, value:URI> type DB
 and returns the label.
*/
char* URI_To_Label(char* uri) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    // if input is NULL => return -1
    if (uri == NULL) {
        fprintf(stderr, "URI is empty\n");
        exit(-1);
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* 0 if LABEL does not exist, 1 if present */
    int flag = 0;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(data.data, uri, data.size) == 0) {
            flag = 1;
            return (char*)key.data;

            //DB close
            dbcp->close(dbcp);
            dbp->close(dbp, 0);
            break;
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        exit(0);
    }
    if (flag == 0)
        return NULL;

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close
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