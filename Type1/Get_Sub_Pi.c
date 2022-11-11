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

Node* Get_Sub_Pi(char* pi){
    char* database = "sub.db";

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
    int cnt_sub = 0;

    // ������Ʈ�� ����� ã�� ���� Ŀ��
    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, pi, key.size) == 0) {
            cnt++; // ��ü ����
        }
    }
    if (cnt == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
        exit(1);
    }

    //������Ʈ ���� ����
    int struct_size = 10;
    cnt = cnt / struct_size;
    char* tmp;


    Node* head = (Node*)calloc(cnt, sizeof(Node));
    Node* node;
    node = head;
    //node_ri = node_pi = node_rn = node_nu = node_sub_bit = head;
    
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, pi, key.size) == 0) {
            switch (idx) {
            case 0:
                node->ri = malloc(data.size);
                strcpy(node->ri, data.data);

                node->siblingRight = (Node*)malloc(sizeof(Node));
                node->siblingRight->siblingLeft = node;

                idx++;
                break;
            case 1:
                node->rn = malloc(data.size);
                strcpy(node->rn, data.data);

                idx++;
                break;
            case 2:
                node->nu = malloc(data.size);
                strcpy(node->nu, data.data);

                idx++;
                break;
            case 3:
                //tmp = malloc(data.size);
                //strcpy(tmp, data.data);
                node->net = NetToBit(data.data);

                //node->net = malloc(data.size);
                //strcpy(node->net, data.data);

                idx++;
                break;
            case 4:
                node->sur = malloc(data.size);
                strcpy(node->sur, data.data);

                idx++;
                break;
            case 5:
                node->pi = malloc(key.size);
                strcpy(node->pi, key.data);

                node = node->siblingRight;
                idx++;
                break;
            default:
                idx++;
                if (idx == struct_size) idx = 0;
            }
            
        }
    }

    node->siblingLeft->siblingRight = NULL;
    free(node);
    node = NULL;

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