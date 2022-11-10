#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include <time.h>
#include "onem2m.h"
double start, end;

Node* Get_Sub_Pi(char* pi);
int main() {

    start = (double)clock() / CLOCKS_PER_SEC;
    Node* sub = Get_Sub_Pi_2("3-20220406084023203796");

    end = (((double)clock()) / CLOCKS_PER_SEC);
    printf("Noti2_time :%lf\n", (end - start));

    if (sub == NULL) printf("test\n");
    else {
        while (sub) {
            fprintf(stderr, "%s %s %s %s %s\n", sub->rn, sub->ri, sub->nu, sub->net, sub->pi);
            sub = sub->siblingRight;
        }

    }

    return 0;
}

Node* Get_Sub_Pi(char* pi) {
    char* database = "SUB.db";

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

    // ������Ʈ�� ����� ã�� ���� Ŀ��
    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "pi", key.size) == 0 && strncmp(data.data, pi, data.size) == 0) {
            cnt++; // ��ü ����
        }
    }
    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    //������Ʈ ������ŭ �����Ҵ�
    arr = (int*)malloc(sizeof(int) * cnt);
    for (int i = 0; i < cnt; i++) arr[i] = 0;
    
    // �ش��ϴ� ������Ʈ�� ����� ã�� ���� Ŀ��
    DBC* dbcp1;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp1, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    // �ش��ϴ� ������Ʈ �迭�� 1�� ǥ��
    while ((ret = dbcp1->get(dbcp1, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "pi", key.size) == 0) {
            if (strncmp(data.data, pi, data.size) == 0 )
                arr[idx] = 1;
            idx++;
        }
    }

    Node* head = (Node*)calloc(cnt, sizeof(Node));
    Node* node_net;
    Node* node_nu;
    Node* node_pi;
    Node* node_ri;
    Node* node_rn;

    node_rn = node_ri = node_nu = node_net = node_pi = head;
    

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "net", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_net->net = malloc(data.size);
                strcpy(node_net->net, data.data);
                node_net->siblingRight = (Node*)malloc(sizeof(Node));
                node_net->siblingRight->siblingLeft = node_net;
                node_net = node_net->siblingRight;
            }
            idx++;
        }
        if (strncmp(key.data, "nu", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_nu->nu = malloc(data.size);
                strcpy(node_nu->nu, data.data);
                node_nu = node_nu->siblingRight;
            }
            idx++;
        }
        if (strncmp(key.data, "pi", key.size) == 0) {
            if (arr[idx % cnt]) {
                node_pi->pi = malloc(data.size);
                strcpy(node_pi->pi, data.data);
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
    }

    node_pi->siblingLeft->siblingRight = NULL;
    free(node_pi);
    node_ri = node_pi = node_rn = node_nu = node_net = NULL;

    /* DB close */
    dbcp->close(dbcp0);
    dbcp->close(dbcp);
    dbp->close(dbp, 0);

    return head;
}

int DB_display(char* database)
{
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
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ) {
            printf("%.*s : %d\n", (int)key.size, (char*)key.data, *(int*)data.data);
        }
        else if (strncmp(key.data, "rr", key.size) == 0) {
            printf("%.*s : ", (int)key.size, (char*)key.data);
            if (*(bool*)data.data == true)
                printf("true\n");
            else
                printf("false\n");
        }

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