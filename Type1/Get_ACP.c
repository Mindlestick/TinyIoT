#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {
    
    double start, end;

    //for 루프 시작 시간
    start = (double)clock() / CLOCKS_PER_SEC; 
    ACP *acp = Get_ACP("ri-14998");

     //for 루프 끝난 시간
    end = (((double)clock()) / CLOCKS_PER_SEC);
    printf("프로그램 수행 시간 :%lf\n", (end-start));
    
    printf("%s\n",acp->rn);

    return 0;
}

ACP* Get_ACP(char* ri) {
    ACP* new_acp = (ACP*)malloc(sizeof(ACP));

    char* database = "ACP_type1.db";
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

    /* Open the database. */
    if ((ret = dbp->open(dbp, NULL, database, NULL,
        DB_UNKNOWN, DB_RDONLY, 0)) != 0) {
        dbp->err(dbp, ret, "%s: DB->open", database);
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

    int idx = 0;
    int cnt = 0;
    /* new Cursor */
    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "ri", key.size) == 0) {
            idx++;
            if (strncmp(data.data, ri, data.size) == 0) {
                cnt++;
                new_acp->ri = malloc(data.size);
                strcpy(new_acp->ri, data.data);
                break;
            }
        }
    }

    /* DB is empty */
    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    int cnt_rn = 0;
    int cnt_pi = 0;
    int cnt_ty = 0;
    int cnt_et = 0;
    int cnt_lt = 0;
    int cnt_ct = 0;
    int cnt_pv_acor = 0;
    int cnt_pv_acop = 0;
    int cnt_pvs_acor = 0;
    int cnt_pvs_acop = 0;


    //store ACP
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {

        if (strncmp(key.data, "pi", key.size) == 0) {
            cnt_pi++;
            if(cnt_pi == idx){
                new_acp->pi = malloc(data.size);
                strcpy(new_acp->pi, data.data);
            }
        }
        if (strncmp(key.data, "rn", key.size) == 0) {
            cnt_rn++;
            if(cnt_rn == idx){
            new_acp->rn = malloc(data.size);
            strcpy(new_acp->rn, data.data);
            }
        }
        if (strncmp(key.data, "ty", key.size) == 0) {
            cnt_ty++;
            if(cnt_ty == idx){
            new_acp->ty = *(int*)data.data;
            }
        }
        if (strncmp(key.data, "lt", key.size) == 0) {
            cnt_lt++;
            if(cnt_lt == idx){
            new_acp->lt = malloc(data.size);
            strcpy(new_acp->lt, data.data);
            }
        }
        if (strncmp(key.data, "ct", key.size) == 0) {
            cnt_ct++;
            if(cnt_ct == idx){
            new_acp->ct = malloc(data.size);
            strcpy(new_acp->ct, data.data);
            }
        }
        if (strncmp(key.data, "et", key.size) == 0) {
            cnt_et++;
            if(cnt_et == idx){
            new_acp->et = malloc(data.size);
            strcpy(new_acp->et, data.data);
            }
        }   

        if (strncmp(key.data, "pv_acor", key.size) == 0) {
            cnt_pv_acor++;
            if(cnt_pv_acor == idx){
            new_acp->pv_acor = malloc(data.size);
            strcpy(new_acp->pv_acor, data.data);
            }
        }     
        if (strncmp(key.data, "pv_acop", key.size) == 0) {
            cnt_pv_acop++;
            if(cnt_pv_acop == idx){
            new_acp->pv_acop = malloc(data.size);
            strcpy(new_acp->pv_acop, data.data);
            }
        }     
        if (strncmp(key.data, "pvs_acor", key.size) == 0) {
            cnt_pvs_acor++;
            if(cnt_pvs_acor == idx){
            new_acp->pvs_acor = malloc(data.size);
            strcpy(new_acp->pvs_acor, data.data);
            }
        }             
        if (strncmp(key.data, "pvs_acop", key.size) == 0) {
            cnt_pvs_acop++;
            if(cnt_pvs_acop == idx){
            new_acp->pvs_acop = malloc(data.size);
            strcpy(new_acp->pvs_acop, data.data);
            }
        }                             
    }

    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

    dbcp->close(dbcp0);
    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close
    return new_acp;
}
