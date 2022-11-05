#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    AE ae_before;
    ae_before.rn = "Sensor2_updateeee";
    ae_before.ri = "TAE2";
    ae_before.api = "tinyProject2_update";

    int flag = DB_Update_AE(&ae_before);
    if(flag)
        display("AE.db");

    return 0;
}

int DB_Update_AE(AE* ae_object) {

    /* ri NULL ERROR*/
    if(ae_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return 0;
    }

    /* Not NULL:0, NULL:1 */
    int rn_f=0, pi_f=0, ct_f=0, lt_f=0, et_f=0, api_f=0, aei_f=0, ty_f=0, rr_f=0;

    if(ae_object->rn==NULL) rn_f=1;
    if(ae_object->pi==NULL) pi_f=1;    
    if(ae_object->ct==NULL) ct_f=1;
    if(ae_object->lt==NULL) lt_f=1;
    if(ae_object->et==NULL) et_f=1;
    if(ae_object->api==NULL) api_f=1;
    if(ae_object->aei==NULL) aei_f=1;
    if(ae_object->ty==0) ty_f=1;  
    if(ae_object->rr==false) rr_f=1;      

    char* database = "AE.db";

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
            if (strncmp(data.data, ae_object->ri, data.size) == 0) {
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

    int cnt_rn = 0,cnt_pi = 0,cnt_et = 0,cnt_lt = 0,cnt_ct = 0,cnt_api = 0,cnt_aei = 0;
    int cnt_rr = 0,cnt_ty = 0;
    
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "rn", key.size) == 0 && rn_f==0) {
            cnt_rn++;
            if (cnt_rn == idx) {
                data.size = strlen(ae_object->rn) + 1;
                strcpy(data.data, ae_object->rn);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "pi", key.size) == 0 && pi_f==0) {
            cnt_pi++;
            if (cnt_pi == idx) {
                data.size = strlen(ae_object->pi) + 1;
                strcpy(data.data, ae_object->pi);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "api", key.size) == 0 && api_f==0) {
            cnt_api++;
            if (cnt_api == idx) {
                data.size = strlen(ae_object->api) + 1;
                strcpy(data.data, ae_object->api);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "aei", key.size) == 0 && aei_f==0) {
            cnt_aei++;
            if (cnt_aei == idx) {
                data.size = strlen(ae_object->aei) + 1;
                strcpy(data.data, ae_object->aei);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "et", key.size) == 0 && et_f==0) {
            cnt_et++;
            if (cnt_et == idx) {
                data.size = strlen(ae_object->et) + 1;
                strcpy(data.data, ae_object->et);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "lt", key.size) == 0 && lt_f==0) {
            cnt_lt++;
            if (cnt_lt == idx) {
                data.size = strlen(ae_object->lt) + 1;
                strcpy(data.data, ae_object->lt);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "ct", key.size) == 0 && ct_f==0) {
            cnt_ct++;
            if (cnt_ct == idx) {
                data.size = strlen(ae_object->ct) + 1;
                strcpy(data.data, ae_object->ct);
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "ty", key.size) == 0 && ty_f==0) {
            cnt_ty++;
            if (cnt_ty == idx) {
                *(int*)data.data = ae_object->ty;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
        if (strncmp(key.data, "rr", key.size) == 0 && rr_f==0) {
            cnt_rr++;
            if (cnt_rr == idx) {
                *(bool*)data.data = ae_object->rr;
                dbcp->put(dbcp, &key, &data, DB_CURRENT);
            }
        }
    }

    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        return 0;
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