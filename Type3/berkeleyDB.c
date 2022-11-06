#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "onem2m.h"
int main() {

    CNT cnt_after;
    cnt_after.rn = "status1_updateeee";
    cnt_after.ri = "3-20220513093154147745";

    int flag = Update_CNT_DB(&cnt_after);
    if(flag)
        display(DATABASE);

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
DB* DB_OPEN_(DB *dbp){
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

/*DB Display*/
int display(char* database)
{
    fprintf(stderr, "[Display] %s \n", database); //DB name print

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


int Store_CSE(CSE *cse_object) {
    //char* DATABASE = "CSE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data


    // if input == NULL
    if (cse_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return 0;
    }
    if (cse_object->rn == NULL) cse_object->rn = " ";
    if (cse_object->pi == NULL) cse_object->pi = "NULL";
    if (cse_object->ty == '\0') cse_object->ty = 0;
    if (cse_object->ct == NULL) cse_object->ct = " ";
    if (cse_object->lt == NULL) cse_object->lt = " ";
    if (cse_object->csi == NULL) cse_object->csi = " ";

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cse_object->ri;
    key_ri.size = strlen(cse_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s",
            cse_object->rn,cse_object->pi,cse_object->ty,cse_object->ct,cse_object->lt,cse_object->csi);

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

int Store_AE(AE *ae_object) {
    //char* DATABASE = "AE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    char rr[6] ="";

    // if input == NULL
    if (ae_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return 0;
    }
    if (ae_object->rn == NULL) ae_object->rn = " ";
    if (ae_object->pi == NULL) ae_object->pi = "NULL";
    if (ae_object->ty == '\0') ae_object->ty = 0;
    if (ae_object->ct == NULL) ae_object->ct = " ";
    if (ae_object->lt == NULL) ae_object->lt = " ";
    if (ae_object->et == NULL) ae_object->et = " ";

    if (ae_object->rr == '\0') ae_object->rr = true;
    else if(ae_object->rr == true) strcpy(rr,"true");
    else strcpy(rr,"false");

    if (ae_object->api == NULL) ae_object->api = " ";
    if (ae_object->aei == NULL) ae_object->aei = " ";

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = ae_object->ri;
    key_ri.size = strlen(ae_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s,%s,%s,%s",
            ae_object->rn,ae_object->pi,ae_object->ty,ae_object->ct,ae_object->lt,
            ae_object->et,ae_object->api,rr,ae_object->aei);

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

int Store_CNT(CNT *cnt_object) {
    //char* DATABASE = "CNT.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    
    // if input == NULL
    if (cnt_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return 0;
    }
    if (cnt_object->rn == NULL) cnt_object->rn = " ";
    if (cnt_object->pi == NULL) cnt_object->pi = "NULL";
    if (cnt_object->ty == '\0') cnt_object->ty = 0;
    if (cnt_object->ct == NULL) cnt_object->ct = " ";
    if (cnt_object->lt == NULL) cnt_object->lt = " ";
    if (cnt_object->et == NULL) cnt_object->et = " ";

    if (cnt_object->cni == '\0') cnt_object->cni = 0;
    if (cnt_object->cbs == '\0') cnt_object->cbs = 0;
    if (cnt_object->st == '\0') cnt_object->st = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cnt_object->ri;
    key_ri.size = strlen(cnt_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s,%d,%d,%d",
            cnt_object->rn,cnt_object->pi,cnt_object->ty,cnt_object->ct,cnt_object->lt,cnt_object->et,
            cnt_object->cbs,cnt_object->cni,cnt_object->st);

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

int Store_CIN(CIN *cin_object) {
    //char* DATABASE = "CIN.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    
    // if input == NULL
    if (cin_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return 0;
    }
    if (cin_object->rn == NULL) cin_object->rn = " ";
    if (cin_object->pi == NULL) cin_object->pi = "NULL";
    if (cin_object->ty == '\0') cin_object->ty = 0;
    if (cin_object->ct == NULL) cin_object->ct = " ";
    if (cin_object->lt == NULL) cin_object->lt = " ";
    if (cin_object->et == NULL) cin_object->et = " ";

    if (cin_object->con == NULL) cin_object->con = " ";
    if (cin_object->csi == NULL) cin_object->csi = " ";
    if (cin_object->cs == '\0') cin_object->cs = 0;
    if (cin_object->st == '\0') cin_object->st = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cin_object->ri;
    key_ri.size = strlen(cin_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s,%s,%s,%d,%d",
            cin_object->rn,cin_object->pi,cin_object->ty,cin_object->ct,cin_object->lt,cin_object->et,
            cin_object->con,cin_object->csi,cin_object->cs,cin_object->st);

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


CSE* Get_CSE(char* ri) {
    //char* DATABASE = "CSE.db";

    //struct to return
    CSE* new_cse = (CSE*)malloc(sizeof(CSE));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cse->ri = malloc(key.size);
            strcpy(new_cse->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_cse->rn = malloc(strlen(ptr));
                    strcpy(new_cse->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_cse->pi = malloc(strlen(ptr));
                    strcpy(new_cse->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_cse->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_cse->ct = malloc(strlen(ptr));
                    strcpy(new_cse->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_cse->lt = malloc(strlen(ptr));
                    strcpy(new_cse->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_cse->csi = malloc(strlen(ptr));
                    strcpy(new_cse->csi, ptr);

                    idx++;
                    break;             
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, ","); //The delimiter is ,
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

    return new_cse;
}

AE* Get_AE(char* ri) {
    //char* DATABASE = "AE.db";

    //struct to return
    AE* new_ae = (AE*)malloc(sizeof(AE));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_ae->ri = malloc(key.size);
            strcpy(new_ae->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_ae->rn = malloc(strlen(ptr));
                    strcpy(new_ae->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_ae->pi = malloc(strlen(ptr));
                    strcpy(new_ae->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_ae->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_ae->ct = malloc(strlen(ptr));
                    strcpy(new_ae->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_ae->lt = malloc(strlen(ptr));
                    strcpy(new_ae->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_ae->et = malloc(strlen(ptr));
                    strcpy(new_ae->et, ptr);

                    idx++;
                    break;      
                case 6:
                    new_ae->api = malloc(strlen(ptr));
                    strcpy(new_ae->api, ptr);

                    idx++;
                    break;      
                case 7:
                    if(strcmp(ptr,"true")==0)
                        new_ae->rr = true;
                    else
                        new_ae->rr = false;

                    idx++;
                    break;      
                case 8:
                    new_ae->aei = malloc(strlen(ptr));
                    strcpy(new_ae->aei, ptr);

                    idx++;
                    break;                                                                     
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, ","); //The delimiter is ,
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

    return new_ae;
}


CNT* Get_CNT(char* ri) {
    //char* DATABASE = "CNT.db";

    //struct to return
    CNT* new_cnt = (CNT*)malloc(sizeof(CNT));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cnt->ri = malloc(key.size);
            strcpy(new_cnt->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_cnt->rn = malloc(strlen(ptr));
                    strcpy(new_cnt->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_cnt->pi = malloc(strlen(ptr));
                    strcpy(new_cnt->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_cnt->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_cnt->ct = malloc(strlen(ptr));
                    strcpy(new_cnt->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_cnt->lt = malloc(strlen(ptr));
                    strcpy(new_cnt->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_cnt->et = malloc(strlen(ptr));
                    strcpy(new_cnt->et, ptr);

                    idx++;
                    break;      
                case 6:
                    new_cnt->cbs = atoi(ptr);

                    idx++;
                    break;     
                case 7:
                    new_cnt->cni = atoi(ptr);

                    idx++;
                    break;    
                case 8:
                    new_cnt->st = atoi(ptr);

                    idx++;
                    break;                                                                    
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, ","); //The delimiter is ,
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

    return new_cnt;
}


CIN* Get_CIN(char* ri) {
    //char* DATABASE = "CIN.db";

    //struct to return
    CIN* new_cin = (CIN*)malloc(sizeof(CIN));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cin = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cin++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cin->ri = malloc(key.size);
            strcpy(new_cin->ri, key.data);

            char *ptr = strtok((char*)data.data, ",");  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                    new_cin->rn = malloc(strlen(ptr));
                    strcpy(new_cin->rn, ptr);

                    idx++;
                    break;
                case 1:
                    new_cin->pi = malloc(strlen(ptr));
                    strcpy(new_cin->pi, ptr);

                    idx++;
                    break;
                case 2:
                    new_cin->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                    new_cin->ct = malloc(strlen(ptr));
                    strcpy(new_cin->ct, ptr);

                    idx++;
                    break;
                case 4:
                    new_cin->lt = malloc(strlen(ptr));
                    strcpy(new_cin->lt, ptr);

                    idx++;
                    break;                
                case 5:
                    new_cin->et = malloc(strlen(ptr));
                    strcpy(new_cin->et, ptr);

                    idx++;
                    break;      
                case 6:
                    new_cin->con = malloc(strlen(ptr));
                    strcpy(new_cin->con, ptr);

                    idx++;
                    break;       
                case 7:
                    new_cin->csi = malloc(strlen(ptr));
                    strcpy(new_cin->csi, ptr);

                    idx++;
                    break;   
                case 8:
                    new_cin->cs = atoi(ptr);

                    idx++;
                    break;            
                case 9:
                    new_cin->st = atoi(ptr);

                    idx++;
                    break;                                                                              
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, ","); //The delimiter is ,
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        exit(0);
    }
    if (cin == 0 || flag==0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);

    return new_cin;
}

int Update_AE_DB(AE* ae_object) {
    DB* dbp;
    DBC* dbcp;
    DBT key_ri, data;
    int ret;
    char rr[6]="";

    /* ri NULL ERROR*/
    if(ae_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return 0;
    }

    //Struct to store in DB
    AE* ae = (AE*)malloc(sizeof(AE));
    ae = Get_AE(ae_object->ri);

    if(ae_object->rn!=NULL) strcpy(ae->rn,ae_object->rn);
    if(ae_object->pi!=NULL) strcpy(ae->pi,ae_object->pi);
    if(ae_object->ct!=NULL) strcpy(ae->ct,ae_object->ct);
    if(ae_object->lt!=NULL) strcpy(ae->lt,ae_object->lt);
    if(ae_object->et!=NULL) strcpy(ae->et,ae_object->et);
    if(ae_object->api!=NULL) strcpy(ae->api,ae_object->api);
    if(ae_object->aei!=NULL) strcpy(ae->aei,ae_object->aei);
    else if(ae_object->rr == true || ae_object->rr == '\0') strcpy(rr,"true");
    else strcpy(rr,"false");
    if(ae_object->ty!=0) ae->ty=ae_object->ty;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = ae_object->ri;
    key_ri.size = strlen(ae_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s,%s,%s,%s",
            ae->rn,ae->pi,ae->ty,ae->ct,ae->lt,
            ae->et,ae->api,rr,ae->aei);

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

int Update_CNT_DB(CNT* cnt_object) {
    DB* dbp;
    DBC* dbcp;
    DBT key_ri, data;
    int ret;
    char rr[6]="";

    /* ri NULL ERROR*/
    if(cnt_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return 0;
    }

    //Struct to store in DB
    CNT* cnt = (CNT*)malloc(sizeof(CNT));
    cnt = Get_CNT(cnt_object->ri);

    if(cnt_object->rn!=NULL) strcpy(cnt->rn,cnt_object->rn);
    if(cnt_object->pi!=NULL) strcpy(cnt->pi,cnt_object->pi);
    if(cnt_object->ct!=NULL) strcpy(cnt->ct,cnt_object->ct);
    if(cnt_object->lt!=NULL) strcpy(cnt->lt,cnt_object->lt);
    if(cnt_object->et!=NULL) strcpy(cnt->et,cnt_object->et);
    if(cnt_object->lbl!=NULL) strcpy(cnt->lbl,cnt_object->lbl);
    if(cnt_object->acpi!=NULL) strcpy(cnt->acpi,cnt_object->acpi);    
    if(cnt_object->ty!=0) cnt->ty=cnt_object->ty;
    if(cnt_object->cbs!=0) cnt->ty=cnt_object->cbs;
    if(cnt_object->cni!=0) cnt->ty=cnt_object->cni;       
    if(cnt_object->st!=0) cnt->ty=cnt_object->st;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cnt_object->ri;
    key_ri.size = strlen(cnt_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s,%s,%s,%d,%d,%d",
            cnt->rn,cnt->pi,cnt->ty,cnt->ct,cnt->lt,
            cnt->et,cnt->lbl,cnt->acpi,cnt->cbs,cnt->cni,cnt->st);

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
