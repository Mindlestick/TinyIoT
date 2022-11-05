#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    Node* cnt = Get_All_CNT();
    while (cnt) {
        fprintf(stderr, "%s\n", cnt->rn);
        cnt = cnt->siblingRight;
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

Node* Get_All_CNT() {
    fprintf(stderr, "[Get All CNT]\n");

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
        exit(1);
    }

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    int cnt = 0;
    // 몇번째 CNT인지 찾기 위한 커서
    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "ri", key.size) == 0) {
            cnt++; // 오브젝트 개수
        }
    }
    //fprintf(stderr, "<%d>\n",cnt);

    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    // cnt 개수만큼 동적할당
    Node* head = (Node*)malloc(sizeof(Node));
    Node* node_ri;
    Node* node_pi;
    Node* node_rn;
    Node* node_ty;

    node_ri = node_pi = node_rn = node_ty = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "pi", key.size) == 0) {
            node_pi->pi = malloc(data.size);
            strcpy(node_pi->pi, data.data);
            node_pi->siblingRight = Create_Node("", "", "", 0);
            node_pi->siblingRight->siblingLeft = node_pi;
            node_pi = node_pi->siblingRight;
        }
        if (strncmp(key.data, "ri", key.size) == 0) {
            node_ri->ri = malloc(data.size);
            strcpy(node_ri->ri, data.data);
            node_ri = node_ri->siblingRight;

        }
        if (strncmp(key.data, "rn", key.size) == 0) {
            node_rn->rn = malloc(data.size);
            strcpy(node_rn->rn, data.data);
            node_rn = node_rn->siblingRight;
        }
        if (strncmp(key.data, "ty", key.size) == 0) {
            node_ty->ty = *(int*)data.data;
            node_ty = node_ty->siblingRight;
        }
    }

    if (node_pi->siblingLeft) node_pi->siblingLeft->siblingRight = NULL;
    else head = NULL;
    Free_Node(node_pi);
    node_ri = node_pi = node_rn = node_ty = NULL;


    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

    return head;
}