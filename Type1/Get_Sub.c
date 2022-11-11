#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

Sub* Get_Sub(char* ri);
int main() {
    Sub *sub1 = Get_Sub("23-2022040684653299304"); 

    printf("%s\n",sub1->sur);
    return 0;
}

Sub* Get_Sub(char* ri) {
    char* database = "SUB.db";

    //store AE
    Sub* new_sub = (Sub*)malloc(sizeof(Sub));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    /* Open the database. */
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr,
            "%s: db_create: %s\n", database, db_strerror(ret));
        return 0;
    }

    ret = dbp->open(dbp, NULL, database, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", database);
        exit(1);
    }

    /* Acquire a cursor for the database. */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    int cnt = 0;
    int flag = 0;
    int struct_size = 10;

    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(data.data, ri, data.size) == 0) {
            flag=1;
            break;
        }
    }
    if (cnt == 0 || flag==0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }


    new_sub->pi = malloc(data.size);
    strcpy(new_sub->pi, data.data);

    int idx = -1;
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(data.data, ri, data.size) == 0) {
            idx=0;
        }
        switch (idx) {
            case 0:
                new_sub->ri = malloc(data.size);
                strcpy(new_sub->ri, data.data);

                idx++;
                break;
            case 1:
                new_sub->rn = malloc(data.size);
                strcpy(new_sub->rn, data.data);

                idx++;
                break;
            case 2:
                new_sub->nu = malloc(data.size);
                strcpy(new_sub->nu, data.data);

                idx++;
                break;
            case 3:
                new_sub->net = malloc(data.size);
                strcpy(new_sub->net, data.data);

                idx++;
                break;
            case 4:
                new_sub->sur = malloc(data.size);
                strcpy(new_sub->sur, data.data);

                idx++;
                break;
            case 5:
                new_sub->ct = malloc(data.size);
                strcpy(new_sub->ct, data.data);

                idx++;
                break;
            case 6:
                new_sub->et = malloc(data.size);
                strcpy(new_sub->et, data.data);

                idx++;
                break;
            case 7:
                new_sub->lt = malloc(data.size);
                strcpy(new_sub->lt, data.data);

                idx++;
                break;
            case 8:
                new_sub->ty = *(int*)data.data;

                idx++;
                break;
            case 9:
                new_sub->nct = *(int*)data.data;

                idx++;
                break;
            default:
                idx=-1;
        }

    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

        /* Cursors must be closed */
    if (dbcp0 != NULL)
        dbcp0->close(dbcp0);
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);

    return new_sub;
}