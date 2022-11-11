#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    Sub sub_after;
    sub_after.rn = "sub3_update";
    sub_after.ri = "23-2023040684653299304";

    int f = Update_Sub(&sub_after);
    if(f) display("SUB.db");

    return 0;
}

int Update_Sub(Sub *sub_object) {

    /* ri NULL ERROR*/
    if(sub_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return 0;
    }

    /* Not NULL:0, NULL:1 */
    int rn_f=0, pi_f=0, ct_f=0, lt_f=0, et_f=0, net_f=0,sur_f=0,nct_f=0,nu_f=0, ty_f=0;

    if(sub_object->rn==NULL) rn_f=1;
    if(sub_object->pi==NULL) pi_f=1;    
    if(sub_object->ct==NULL) ct_f=1;
    if(sub_object->lt==NULL) lt_f=1;
    if(sub_object->et==NULL) et_f=1;
    if(sub_object->nu==NULL) nu_f=1;
    if(sub_object->net==NULL) net_f=1;
    if(sub_object->sur==NULL) sur_f=1;
    if(sub_object->ty==0) ty_f=1; 
    if(sub_object->nct==0) nct_f=1;


    char* database = "SUB.db";

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
        return 0;
    }

    /* Acquire a cursor for the database. */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        return 0;
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
        return 0;
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(data.data, sub_object->ri, data.size) == 0) {
            flag=1;
            break;
        }
    }
    if (cnt == 0 || flag==0) {
        fprintf(stderr, "Data not exist\n");
        return 0;
    }

    int idx = -1;
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(data.data, sub_object->ri, data.size) == 0) {
            idx=0;
        }
        if(idx!=-1 && idx < struct_size){
            // idx==0 -> ri
            if(idx==1 && rn_f==0) {
                data.size = strlen(sub_object->rn) + 1;
                strcpy(data.data, sub_object->rn);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==2 && nu_f==0) {
                data.size = strlen(sub_object->nu) + 1;
                strcpy(data.data, sub_object->nu);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==3 && net_f==0) {
                data.size = strlen(sub_object->net) + 1;
                strcpy(data.data, sub_object->net);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }            
            if(idx==4 && sur_f==0) {
                data.size = strlen(sub_object->sur) + 1;
                strcpy(data.data, sub_object->sur);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==5 && ct_f==0) {
                data.size = strlen(sub_object->ct) + 1;
                strcpy(data.data, sub_object->ct);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==6 && lt_f==0) {
                data.size = strlen(sub_object->lt) + 1;
                strcpy(data.data, sub_object->lt);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==7 && et_f==0) {
                data.size = strlen(sub_object->et) + 1;
                strcpy(data.data, sub_object->et);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==8 && ty_f==0) {
                *(int*)data.data=sub_object->ty;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            if(idx==9 && nct_f==0) {
                *(int*)data.data=sub_object->nct;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
            idx++;
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return 0;
    }

    /* Cursors must be closed */
    if (dbcp0 != NULL)
        dbcp0->close(dbcp0);
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);
        
    /* Delete Success */
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
            printf("%.*s : %d\n", (int)key.size, (char*)key.data, *(int*)data.data);
        }
        //bool
        else if (strncmp(key.data, "rr", key.size) == 0) {
            printf("%.*s : ", (int)key.size, (char*)key.data);
            if (*(bool*)data.data == true)
                printf("true\n");
            else
                printf("false\n");
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