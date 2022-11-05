#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

ACP* Get_ACP(char* ri);
int main() {
    /*
    double start, end;

    //for 루프 시작 시간
    start = (double)clock() / CLOCKS_PER_SEC; 
    ACP *acp = Get_ACP("ri-14998");

     //for 루프 끝난 시간
    end = (((double)clock()) / CLOCKS_PER_SEC);
    printf("프로그램 수행 시간 :%lf\n", (end-start));

    */
    ACP *acp = Get_ACP("1-20191210093452845");
    printf("%s\n",acp->rn);

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

ACP* Get_ACP(char* ri) {
    //char* DATABASE = "ACP.db";

    //struct to return
    ACP* new_acp = (ACP*)malloc(sizeof(ACP));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_acp->ri = malloc(key.size);
            strcpy(new_acp->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_acp->rn = malloc(strlen(ptr));
                    strcpy(new_acp->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_acp->pi = malloc(strlen(ptr));
                    strcpy(new_acp->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_acp->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_acp->ct = malloc(strlen(ptr));
                    strcpy(new_acp->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_acp->lt = malloc(strlen(ptr));
                    strcpy(new_acp->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_acp->et = malloc(strlen(ptr));
                    strcpy(new_acp->et, ptr);

                    idx++;
                    break;
                case 6:
                    new_acp->pv_acor = malloc(strlen(ptr));
                    strcpy(new_acp->pv_acor, ptr);

                    idx++;
                    break;
                case 7:
                    new_acp->pv_acop = malloc(strlen(ptr));
                    strcpy(new_acp->pv_acop, ptr);

                    idx++;
                    break;
                case 8:
                    new_acp->pvs_acor = malloc(strlen(ptr));
                    strcpy(new_acp->pvs_acor, ptr);

                    idx++;
                    break;
                case 9:
                    new_acp->pvs_acop = malloc(strlen(ptr));
                    strcpy(new_acp->pvs_acop, ptr);

                    idx++;
                    break;                
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, ","); //The delimiter is ,
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }
    if (cnt == 0 || flag==0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);

    return new_acp;
}