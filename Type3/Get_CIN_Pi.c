#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"


int main() {
    Node* cin = DB_Get_CIN_Pi("3-20220513091700249586");
    if (cin == NULL) printf("test NULL\n");
    else {
        while (cin) {
            fprintf(stderr, "%s %s %s %s\n", cin->rn, cin->ri, cin->nu, cin->pi);
            cin = cin->siblingRight;
        }
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

Node* DB_Get_CIN_Pi(char* pi) {
    char* DATABASE = "RESOURCE.db";
    char* TYPE = "4-";

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
    int idx = 0;
    int* arr = NULL;

    DBC* dbcp0;
    dbcp0 = DB_GET_CURSOR(dbp,dbcp0);

    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        // find CIN
        if (strncmp(key.data, TYPE, 2) == 0) {
            CIN *cin = DB_Get_CIN((char*)key.data);
            //find pi
            if(strncmp(pi, cin->pi, strlen(pi)) == 0)
                cnt++;
            free(cin);
        }
    }
    fprintf(stderr, "<%d>\n",cnt);

    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }
    Node* head = calloc(cnt,sizeof(Node));
    Node* node;
    node = head;
    
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        //find CIN
        if (strncmp(key.data, TYPE , 2) == 0){
            CIN *cin = DB_Get_CIN((char*)key.data);
            //find pi
            if(strncmp(pi, cin->pi, strlen(pi)) == 0){
                node->ri = calloc(strlen(cin->ri)+1,sizeof(char));
                node->rn = calloc(strlen(cin->rn)+1,sizeof(char));
                node->pi = calloc(strlen(cin->pi)+1,sizeof(char));

                strcpy(node->ri,cin->ri);
                strcpy(node->rn,cin->rn);
                strcpy(node->pi,cin->pi);
                node->ty = cin->ty;

                node->siblingRight=calloc(1,sizeof(Node));            
                node->siblingRight->siblingLeft = node;
                node = node->siblingRight;
            }
            free(cin);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
    }    

    node->siblingLeft->siblingRight = NULL;
    free(node);
    free(node->rn);
    free(node->pi);
    free(node->ri);            
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbcp != NULL)
        dbcp->close(dbcp0);          
    if (dbp != NULL)
        dbp->close(dbp, 0); 

    return head;
}
