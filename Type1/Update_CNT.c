#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    CNT cnt_after;
    cnt_after.rn = "status2_update";
    cnt_after.ri = "3-20210513093154147745";

    int f = DB_Update_CNT(&cnt_after);
    if(f)
        display("CNT.db");

    return 0;
}

int DB_Update_CNT(CNT* cnt_object) {

    /* ri NULL ERROR*/
    if(cnt_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return 0;
    }

    /* Not NULL:0, NULL:1 */
    int rn_f=0, pi_f=0, ct_f=0, lt_f=0, et_f=0, st_f=0, cni_f=0, ty_f=0, cbs_f=0;

    if(cnt_object->rn==NULL) rn_f=1;
    if(cnt_object->pi==NULL) pi_f=1;    
    if(cnt_object->ct==NULL) ct_f=1;
    if(cnt_object->lt==NULL) lt_f=1;
    if(cnt_object->et==NULL) et_f=1;
    if(cnt_object->st==0) st_f=1;
    if(cnt_object->ty==0) ty_f=1; 
    if(cnt_object->cni==0) cni_f=1;
    if(cnt_object->cbs==0) cbs_f=1;   

    char* database = "CNT.db";

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
        return 0;
        exit(1);
    }

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    int cnt = 0;
    int idx = 0;

    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        return 0;
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "ri", key.size) == 0) {
            idx++;
            if (strncmp(data.data, cnt_object->ri, data.size) == 0) {
                cnt++; 
                break;
            }
        }
    }

    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return 0;
        exit(1);
    }

    int cnt_rn = 0;
    int cnt_pi = 0;
    int cnt_ty = 0;
    int cnt_et = 0;
    int cnt_lt = 0;
    int cnt_ct = 0;
    int cnt_cni = 0;
    int cnt_cbs = 0;
    int cnt_st = 0;

    
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0 && rn_f==0) {
        if (strncmp(key.data, "rn", key.size) == 0 && rn_f==0) {
            cnt_rn++;
            if (cnt_rn == idx) {
                data.size = strlen(cnt_object->rn) + 1;
                strcpy(data.data, cnt_object->rn);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }

        if (strncmp(key.data, "pi", key.size) == 0 && pi_f==0) {
            cnt_pi++;
            if (cnt_pi == idx) {
                data.size = strlen(cnt_object->pi) + 1;
                strcpy(data.data, cnt_object->pi);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "et", key.size) == 0 && et_f==0) {
            cnt_et++;
            if (cnt_et == idx) {
                data.size = strlen(cnt_object->et) + 1;
                strcpy(data.data, cnt_object->et);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "lt", key.size) == 0 && lt_f==0) {
            cnt_lt++;
            if (cnt_lt == idx) {
                data.size = strlen(cnt_object->lt) + 1;
                strcpy(data.data, cnt_object->lt);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "ct", key.size) == 0 && ct_f==0) {
            cnt_ct++;
            if (cnt_ct == idx) {
                data.size = strlen(cnt_object->ct) + 1;
                strcpy(data.data, cnt_object->ct);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "ty", key.size) == 0 && ty_f==0) {
            cnt_ty++;
            if (cnt_ty == idx) {
                *(int*)data.data = cnt_object->ty;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "cni", key.size) == 0 && cni_f==0) {
            cnt_cni++;
            if (cnt_cni == idx) {
                *(int*)data.data = cnt_object->cni;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "cbs", key.size) == 0 && cbs_f==0) {
            cnt_cbs++;
            if (cnt_cbs == idx) {
                *(int*)data.data = cnt_object->cbs;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "st", key.size) == 0 && st_f==0) {
            cnt_st++;
            if (cnt_st == idx) {
                *(int*)data.data = cnt_object->st;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
    }

    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        return 0;
        exit(0);
    }

    /* DB close */
    dbcp->close(dbcp);
    dbcp->close(dbcp0);
    dbp->close(dbp, 0); 

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