#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    Node* ae = DB_Get_All_AE();
    while (ae) {
        //fprintf(stderr, "%s\n", ae->rn);
        //ae = ae->siblingRight;
    }

    return 0;
}
void Free_Node(Node* node) {
    free(node->ri);
    free(node->rn);
    free(node->pi);
    free(node);
}
Node* Create_Node(char* ri, char* rn, char* pi, ObjectType ty) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->rn = (char*)malloc(sizeof(rn));
    node->ri = (char*)malloc(sizeof(ri));
    node->pi = (char*)malloc(sizeof(pi));
    strcpy(node->rn, rn);
    strcpy(node->ri, ri);
    strcpy(node->pi, pi);
    node->parent = NULL;
    node->child = NULL;
    node->siblingLeft = NULL;
    node->siblingRight = NULL;
    node->ty = ty;
    if (strcmp(rn, "") && strcmp(rn, "TinyIoT")) {
        fprintf(stderr, "\nCreate Tree Node\n[rn] %s\n[ri] %s\n", node->rn, node->ri);
    }
    return node;
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

Node* DB_Get_All_AE() {
    char* DATABASE = "RESOURCE.db";
    char* TYPE = "2-";

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

    int cnt = 0;
    DBC* dbcp0;
    dbcp0 = DB_GET_CURSOR(dbp,dbcp0);
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0) 
            cnt++;
    }
    //fprintf(stderr, "<%d>\n",cnt);

    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    Node* head = calloc(cnt,sizeof(Node));
    Node* node;
    node = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0){
            AE* ae = DB_Get_AE((char*)key.data);
            node->ri = calloc(strlen(ae->ri)+1,sizeof(char));
            node->rn = calloc(strlen(ae->rn)+1,sizeof(char));
            node->pi = calloc(strlen(ae->pi)+1,sizeof(char));
            //node->ty = calloc(sizeof(ae->ty),sizeof(char));

            strcpy(node->ri,ae->ri);
            strcpy(node->rn,ae->rn);
            strcpy(node->pi,ae->pi);
            node->ty = ae->ty;

            //node->siblingRight=(Node*)calloc(1,sizeof(Node));
            node->siblingRight=calloc(1,sizeof(Node));            
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;
            free(ae);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
    }

    node->siblingLeft->siblingRight = NULL;
    free(node);
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
}