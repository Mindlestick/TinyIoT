#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int net_to_bit(char *net){
    int netLen = strlen(net);
    int ret=0;

    for(int i=0;i<netLen;i++){
        int exp = atoi(net+1);
        if(exp > 0) ret = (ret | (int)pow(2, exp - 1));
    }
    
    return ret;
}

int main() {

    Node* sub = DB_Get_All_Sub();
    while (sub) {
        fprintf(stderr, "%s\n", sub->rn);
        sub = sub->siblingRight;
    }

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

Node* DB_Get_All_Sub() {
    fprintf(stderr,"\x1b[92m[Get All Sub]\x1b[0m\n");    
    char* DATABASE = "Sub.db";
    char* TYPE = "23-";

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    int sub = 0;
    DBC* dbcp0;
    dbcp0 = DB_GET_CURSOR(dbp,dbcp0);
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 3) == 0) 
            sub++;
    }
    //fprintf(stderr, "<%d>\n",sub);

    if (sub == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    Node* head = calloc(sub,sizeof(Node));
    Node* node;
    node = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 3) == 0){
            Sub* sub = DB_Get_Sub((char*)key.data);
            node->ri = calloc(strlen(sub->ri)+1,sizeof(char));
            node->rn = calloc(strlen(sub->rn)+1,sizeof(char));
            node->pi = calloc(strlen(sub->pi)+1,sizeof(char));
            node->nu = calloc(strlen(sub->nu)+1,sizeof(char));

            strcpy(node->ri,sub->ri);
            strcpy(node->rn,sub->rn);
            strcpy(node->pi,sub->pi);
            strcpy(node->rn,sub->nu);    
            node->ty = sub->ty;
            node->net = net_to_bit(sub->net);            

            node->siblingRight=calloc(1,sizeof(Node));            
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;
            free(sub);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
    }

    node->siblingLeft->siblingRight = NULL;
    free(node->ri);
    free(node->rn);
    free(node->pi);
    free(node->nu);     
    free(node);
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
}