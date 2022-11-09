#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include <ctype.h>
#include <time.h>
#include "onem2m.h"

/* Function to create test data */
ACP* StructGenerator(){
    ACP* acp = (ACP*) malloc(25000*sizeof(ACP));
    for(int i=0;i<25000;i++){
        acp[i].rn = (char*)malloc(8);
        acp[i].ri = (char*)malloc(8);        
        char num[5]="";

        strcpy(acp[i].rn, "acp");
        strcpy(acp[i].ri, "ri-");

        sprintf(num, "%d", i);
        strcat(acp[i].rn,num);
        strcat(acp[i].ri,num);
    }
    return acp;
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

int main() {
/*
    double start, end;

    //for 루프 시작 시간
    start = (double)clock() / CLOCKS_PER_SEC; 

    ACP* test = StructGenerator();
    for(int i=0;i<25000;i++){
        Store_ACP(&test[i]);
    }

    //for 루프 끝난 시간
    end = (((double)clock()) / CLOCKS_PER_SEC);
    printf("프로그램 수행 시간 :%lf\n", (end-start));

*/
    
    ACP acp;

    //input sample
    acp.rn = "acp2";
    acp.ri = "1-20191210093452845";
    acp.pi = "pi_pi";
    acp.ty = 1;
    acp.ct = "20191210T093452";
    acp.lt = "20191210T093452";
    acp.et = "20211210T093452";
    acp.pv_acor = "CAE1";
    acp.pv_acop = "2";
    acp.pvs_acor = "SM";
    acp.pvs_acop = "63";

    // [success -> 1] 
    if(DB_Store_ACP(&acp)) fprintf(stderr, "store success!\n");

    // print
    char* DATABASE = "ACP.db";
    DB_display(DATABASE);

    return 0;
}

int DB_Store_ACP(ACP *acp_object) {
    char* DATABASE = "ACP.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data


    // if input == NULL
    if (acp_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (acp_object->rn == NULL) acp_object->rn = " ";
    if (acp_object->pi == NULL) acp_object->pi = " ";
    if (acp_object->ty == '\0') acp_object->ty = 0;
    if (acp_object->ct == NULL) acp_object->ct = " ";
    if (acp_object->lt == NULL) acp_object->lt = " ";
    if (acp_object->et == NULL) acp_object->et = " ";

   if (acp_object->pv_acor == NULL) acp_object->pv_acor = " ";       
   if (acp_object->pv_acop == NULL) acp_object->pv_acop = " "; 
   if (acp_object->pvs_acop == NULL) acp_object->pvs_acor = " "; 
   if (acp_object->pvs_acop == NULL) acp_object->pvs_acop = " "; 
  

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* keyand data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = acp_object->ri;
    key_ri.size = strlen(acp_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    //strcat(str,acp_object->rn);
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%s;%s",
            acp_object->rn,acp_object->pi,acp_object->ty,acp_object->ct,acp_object->lt,
            acp_object->et,acp_object->pv_acor,acp_object->pv_acop,acp_object->pvs_acor,acp_object->pvs_acop);
            
    data.data = str;
    data.size = strlen(str) + 1;

    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_ri, &data, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    /* DB close */
    dbcp->close(dbcp);
    dbp->close(dbp, 0); 

    return 1;
}

int DB_display(char* database)
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
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ){
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