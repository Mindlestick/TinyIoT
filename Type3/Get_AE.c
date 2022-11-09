#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {
    AE *ae = DB_Get_AE("TAE1");
    if(ae->aei!=NULL)
        printf("%s\n",ae->aei);
    else printf("NULL\n");

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

AE* DB_Get_AE(char* ri) {
    char* DATABASE = "RESOURCE.db";

    //struct to return
    AE* new_ae = calloc(1,sizeof(AE));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_ae->ri = calloc(key.size,sizeof(char));
            strcpy(new_ae->ri, key.data);

            char *ptr = strtok((char*)data.data,DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                if(strcmp(ptr," ")==0) new_ae->rn=NULL; //data is NULL
                else{
                    new_ae->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->rn, ptr);
                }
                    idx++;
                    break;
                case 1:
                if(strcmp(ptr," ")==0) new_ae->pi=NULL; //data is NULL
                    else{
                    new_ae->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strcmp(ptr,"0")==0) new_ae->ty=0;
                else new_ae->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                if(strcmp(ptr," ")==0) new_ae->ct=NULL; //data is NULL
                else{
                    new_ae->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->ct, ptr);
                }
                    idx++;
                    break;
                case 4:
                if(strcmp(ptr," ")==0) new_ae->lt=NULL; //data is NULL
                else{                
                    new_ae->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->lt, ptr);
                }
                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_ae->et=NULL; //data is NULL
                else{                
                    new_ae->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->et, ptr);
                }
                    idx++;
                    break;      
                case 6:
                if(strcmp(ptr," ")==0) new_ae->api=NULL; //data is NULL
                else{                
                    new_ae->api = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->api, ptr);
                }
                    idx++;
                    break;      
                case 7:
                    if(strcmp(ptr,"true")==0)
                        new_ae->rr = true;
                    else
                        new_ae->rr = false;

                    idx++;
                    break;      
                case 8:
                if(strcmp(ptr," ")==0) new_ae->aei=NULL; //data is NULL
                else{                
                    new_ae->aei = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->aei, ptr);
                }
                    idx++;
                    break;                                                                     
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ,
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

    return new_ae;
}