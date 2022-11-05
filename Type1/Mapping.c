#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"
char* Find_ri(char* database, char* rn); // ã�� rn�� �ش��ϴ� ri�� ��ȯ�ϴ� �Լ�
Node* Mapping(char uri[]); // uri�� ���� rn�� ri�� ������ Mapping.db�� �����ϰ�, ���� ��ȯ�ϴ� �Լ�
int Store_map(char* key_str, char* data_str); // Mapping.db�� Ű,�� ���·� �����ϴ� �Լ�

int main() {
    //4-20210513093154147745
    char uri[] = "TinyProject/Sensor1/status2/4-20210513093154147745";
    fprintf(stderr, "<URI : %s>\n", uri);

    Node* map = Mapping(uri);
    while (map) {
        fprintf(stderr, "%s : %s\n", map->rn, map->ri);
        map = map->siblingRight;
    }

    display("Mapping.db");
    return 0;
}

Node* Mapping(char uri[]) {
    Node* head = (Node*)malloc(sizeof(Node));
    Node* node;
    node = head;

    int flag = 1;
    char* cse_ri;
    char* ae_ri;
    char* cnt_ri;
    char* cin_ri;

    char* ptr = strtok(uri, "/");
    while (ptr != NULL) {
        //printf("%s\n", ptr);

        // 1: CSE, 2: AE, 3: CNT or CIN, 4: CNT or CIN, default: ERROR
        switch (flag) {
        //CSE
        case 1:
            //printf("CSE ");
            //��ȯ�� ��忡 �߰��ϴ� ����
            node->rn = malloc(sizeof(ptr) + 1);
            strcpy(node->rn, ptr);

            //rn�� �ش��ϴ� ri ã��
            cse_ri = malloc(sizeof(Find_ri("CSE.db", ptr))); 
            strcpy(cse_ri, Find_ri("CSE.db", ptr));
            node->ri = malloc(sizeof(cse_ri) + 1);
            strcpy(node->ri, cse_ri);

            node->siblingRight = (Node*)malloc(sizeof(Node));
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;

            //Mapping.db�� ����
            Store_map(ptr, cse_ri); 
            //printf("[%s : %s]\n", ptr, cse_ri);

            flag++;
            break;

        //AE
        case 2:
            //printf("AE ");

            //��ȯ�� ��忡 �߰��ϴ� ����
            node->rn = malloc(sizeof(ptr) + 1);
            strcpy(node->rn, ptr);

            //rn�� �ش��ϴ� ri ã��
            ae_ri = malloc(sizeof(Find_ri("AE.db", ptr)));
            strcpy(ae_ri, Find_ri("AE.db", ptr));
            node->ri = malloc(sizeof(ae_ri) + 1);
            strcpy(node->ri, ae_ri);

            node->siblingRight = (Node*)malloc(sizeof(Node));
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;

            //Mapping.db�� ����
            Store_map(ptr, ae_ri);

            //printf("[%s : %s]\n", ptr, ae_ri);
            flag++;
            break;

        //CNT
        case 3:
            //printf("CNT ");

            //��ȯ�� ��忡 �߰��ϴ� ����
            node->rn = malloc(sizeof(ptr) + 1);
            strcpy(node->rn, ptr);

            //rn�� �ش��ϴ� ri ã��
            cnt_ri = malloc(sizeof(Find_ri("CNT.db", ptr)));
            strcpy(cnt_ri, Find_ri("CNT.db", ptr));
            node->ri = malloc(sizeof(cnt_ri) + 1);
            strcpy(node->ri, cnt_ri);

            node->siblingRight = (Node*)malloc(sizeof(Node));
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;

            //Mapping.db�� ����
            Store_map(ptr, cnt_ri);

            //printf("[%s : %s]\n", ptr, cnt_ri);
            flag++;
            break;

        //CNT or CIN
        case 4:
            //printf("CNT or CIN ");

            //4��° ���� rn�� CNT�̸�
            if (Find_ri("CIN.db", ptr) == NULL) {
                //��ȯ�� ��忡 �߰��ϴ� ����
                node->rn = malloc(sizeof(ptr) + 1);
                strcpy(node->rn, ptr);

                //rn�� �ش��ϴ� ri ã��
                cnt_ri = malloc(sizeof(Find_ri("CNT.db", ptr)));
                strcpy(cnt_ri, Find_ri("CNT.db", ptr));
                node->ri = malloc(sizeof(cnt_ri) + 1);
                strcpy(node->ri, cnt_ri);

                node->siblingRight = (Node*)malloc(sizeof(Node));
                node->siblingRight->siblingLeft = node;
                node = node->siblingRight;

                //Mapping.db�� ����
                Store_map(ptr, cnt_ri);

                //printf("[%s : %s]\n", ptr, cnt_ri);
                break;
            }
            //4��° ���� rn�� CIN�̸�
            else {
                //��ȯ�� ��忡 �߰��ϴ� ����
                node->rn = malloc(sizeof(ptr) + 1);
                strcpy(node->rn, ptr);

                //rn�� �ش��ϴ� ri ã��
                cin_ri = malloc(sizeof(Find_ri("CIN.db", ptr)));
                strcpy(cin_ri, Find_ri("CIN.db", ptr));
                node->ri = malloc(sizeof(cin_ri) + 1);
                strcpy(node->ri, cin_ri);

                node->siblingRight = (Node*)malloc(sizeof(Node));
                node->siblingRight->siblingLeft = node;
                node = node->siblingRight;

                //Mapping.db�� ����
                Store_map(ptr, cin_ri);

                //printf("[%s : %s]\n", ptr, cin_ri);
                break;
            }
        default:
            fprintf(stderr, "Flag ERROR\n");
            exit(0);
        }
        ptr = strtok(NULL, "/");
    }

    node->siblingLeft->siblingRight = NULL;
    free(node);

    return head;
}

char* Find_ri(char* database, char* rn) {

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

    /* Open the database. */
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

    int idx = 0;
    int flag = 0;
    // ���° ������Ʈ���� ã�� ���� Ŀ��
    DBC* dbcp0;
    if ((ret = dbp->cursor(dbp, NULL, &dbcp0, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        exit(1);
    }
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "rn", key.size) == 0) {
            idx++;
            if (strncmp(data.data, rn, data.size) == 0) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        //printf("Not Found\n");
        return NULL;
        //exit(1);
    }
    //printf("<%d>\n", idx);

    char* ri = NULL;
    int cnt_ri = 0;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "ri", key.size) == 0) {
            cnt_ri++;
            if (cnt_ri == idx) {
                ri = malloc(data.size);
                strcpy(ri, data.data);
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }

    /* Cursors must be closed */
    if (dbcp0 != NULL)
        dbcp0->close(dbcp0);
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);

    return ri;
}

int Store_map(char* key_str, char* data_str) {
    char* database = "Mapping.db";

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

    /* Open the database. */
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

    // Store key & data
    data.data = data_str;
    data.size = strlen(data_str) + 1;
    key.data = key_str;
    key.size = strlen(key_str) + 1;

    if ((ret = dbcp->put(dbcp, &key, &data, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "DB->cursor");

    //DB close
    dbcp->close(dbcp);
    dbp->close(dbp, 0);

    return 1;
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