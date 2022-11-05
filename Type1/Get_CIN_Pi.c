#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    Node* cin = Get_CIN_Pi("3-20220513091700249586");
    while (cin) {
        fprintf(stderr, "%s\n", cin->rn);
        cin = cin->siblingRight;
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

Node* Get_CIN_Pi(char* pi) {

    char* database = "CIN.db";

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
    int idx = 0;
    int* arr = NULL;
    int sum = 0;
    // 오브젝트가 몇개인지 찾기 위한 커서
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
    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    //오브젝트 개수만큼 동적할당
    arr = (int*)malloc(sizeof(int) * cnt);
    for (int i = 0; i < cnt; i++) arr[i] = 0;

    // 해당하는 오브젝트가 몇개인지 찾기 위한 커서
    DBC* dbcp1;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp1, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }

    // 해당하는 오브젝트 배열에 1로 표시
    while ((ret = dbcp1->get(dbcp1, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "pi", key.size) == 0) {
            if (strcmp(pi,data.data)== 0)
                arr[idx] = 1;
            idx++;
        }
    }

    //해당하는 오브젝트가 없음
    for (int i = 0; i < cnt; i++) {
        sum += arr[i];
    }
    if (sum == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    Node* head = (Node*)malloc(sizeof(Node));
    Node* node_ri;
    Node* node_pi;
    Node* node_rn;
    Node* node_ty;

    node_ri = node_pi = node_rn = node_ty = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "pi", key.size) == 0) {
            if (arr[idx % cnt]) {
                //printf("[%d]", idx % cnt);
                node_pi->pi = malloc(data.size);
                strcpy(node_pi->pi, data.data);
                node_pi->siblingRight = Create_Node("","","",0);
                node_pi->siblingRight->siblingLeft = node_pi;
                node_pi = node_pi->siblingRight;
            }
            idx++;
        }
        if (strncmp(key.data, "ri", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_ri->ri = malloc(data.size);
                strcpy(node_ri->ri, data.data);
                node_ri = node_ri->siblingRight;
            }
            idx++;
        }
        if (strncmp(key.data, "rn", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_rn->rn = malloc(data.size);
                strcpy(node_rn->rn, data.data);
                node_rn = node_rn->siblingRight;
            }
            idx++;
        }
        if (strncmp(key.data, "ty", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_ty->ty = *(int*)data.data;
                node_ty = node_ty->siblingRight;
            }
            idx++;
        }

    }

    if (node_pi->siblingLeft) node_pi->siblingLeft->siblingRight = NULL;
    else head = NULL;
    Free_Node(node_pi);
    node_ri = node_pi = node_rn = node_ty = NULL;
    free(arr);

    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

    return head;
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