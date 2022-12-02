#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"
#define DB_STR_MAX 2048
#define DB_SEP ";"

int main() {
    Sub *sub = DB_Get_Sub("23-2021040684653299304");
    if(sub->rn!=NULL)
        printf("%s\n",sub->rn);
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
        return NULL;
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
        return NULL;
    }
    return dbcp;
}

Sub* DB_Get_Sub(char* ri) {
    char* DATABASE = "Sub.db";

    //struct to return
    Sub* new_sub = calloc(1,sizeof(Sub));

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
            new_sub->ri = calloc(key.size,sizeof(char));
            strcpy(new_sub->ri, key.data);

            char *ptr = strtok((char*)data.data,DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                if(strncmp(ptr," ",1)==0) new_sub->rn=NULL; //data is NULL
                else{
                    new_sub->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->rn, ptr);
                }
                    idx++;
                    break;
                case 1:
                if(strncmp(ptr," ",1)==0) new_sub->pi=NULL; //data is NULL
                    else{
                    new_sub->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strncmp(ptr," ",1)==0) new_sub->nu=NULL; //data is NULL
                else{
                    new_sub->nu = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->nu, ptr);
                }
                    idx++;
                    break;  
                case 3:
                if(strncmp(ptr," ",1)==0) new_sub->net=NULL; //data is NULL
                else{
                    new_sub->net = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->net, ptr);
                }
                    idx++;
                    break;                                      
                case 5:
                if(strncmp(ptr,"0",1)==0) new_sub->ty=0;
                else new_sub->ty = atoi(ptr);

                    idx++;
                    break;
                case 6:
                if(strncmp(ptr," ",1)==0) new_sub->ct=NULL; //data is NULL
                else{
                    new_sub->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->ct, ptr);
                }
                    idx++;
                    break;
                case 7:
                if(strncmp(ptr," ",1)==0) new_sub->lt=NULL; //data is NULL
                else{                
                    new_sub->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->lt, ptr);
                }
                    idx++;
                    break;                
                case 8:
                if(strncmp(ptr," ",1)==0) new_sub->et=NULL; //data is NULL
                else{                
                    new_sub->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_sub->et, ptr);
                }
                    idx++;
                    break;
                case 9:
                if(strncmp(ptr,"0",1)==0) new_sub->nct=0;
                else new_sub->nct = atoi(ptr);

                    idx++;
                    break;                                                                                                 
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ;
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

    return new_sub;
}