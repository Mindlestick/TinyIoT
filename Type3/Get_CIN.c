#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

CIN* Get_CIN(char* ri);
int main() {
    CIN *cin = Get_CIN("4-20220808T113154");
    printf("%s\n",cin->csi);

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

CIN* Get_CIN(char* ri) {
    //char* DATABASE = "CIN.db";

    //struct to return
    CIN* new_cin = (CIN*)malloc(sizeof(CIN));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cin = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cin++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cin->ri = malloc(key.size);
            strcpy(new_cin->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_cin->rn = malloc(strlen(ptr));
                    strcpy(new_cin->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_cin->pi = malloc(strlen(ptr));
                    strcpy(new_cin->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_cin->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_cin->ct = malloc(strlen(ptr));
                    strcpy(new_cin->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_cin->lt = malloc(strlen(ptr));
                    strcpy(new_cin->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_cin->et = malloc(strlen(ptr));
                    strcpy(new_cin->et, ptr);

                    idx++;
                    break;      
                case 6:
                    new_cin->con = malloc(strlen(ptr));
                    strcpy(new_cin->con, ptr);

                    idx++;
                    break;       
                case 7:
                    new_cin->csi = malloc(strlen(ptr));
                    strcpy(new_cin->csi, ptr);

                    idx++;
                    break;   
                case 8:
                    new_cin->cs = atoi(ptr);

                    idx++;
                    break;            
                case 9:
                    new_cin->st = atoi(ptr);

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
    if (cin == 0 || flag==0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);

    return new_cin;
}