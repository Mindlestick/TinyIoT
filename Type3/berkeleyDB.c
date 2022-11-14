#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "onem2m.h"


int main(){
    AE ae_after;
    ae_after.rn = "Sensor2_updatee";
    ae_after.ri = "2-20191210093452845";
    ae_after.rr = true;
    ae_after.api = "tinyProject2_update";

    int flag = DB_Update_AE(&ae_after);
    if(flag==1)
        DB_display("RESOURCE.db");

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

/*DB Display*/
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
        return -1;
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
        return -1;
    }


err:    if (close_dbc && (ret = dbcp->close(dbcp)) != 0)
dbp->err(dbp, ret, "DBcursor->close");
if (close_db && (ret = dbp->close(dbp, 0)) != 0)
fprintf(stderr,
    "%s: DB->close: %s\n", database, db_strerror(ret));
return -1;
}


int DB_Store_CSE(CSE *cse_object) {
    char* DATABASE = "RESOURCE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data


    // if input == NULL
    if (cse_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (cse_object->rn == NULL) cse_object->rn = " ";
    if (cse_object->pi == NULL) cse_object->pi = " ";
    if (cse_object->ty == '\0') cse_object->ty = 0;
    if (cse_object->ct == NULL) cse_object->ct = " ";
    if (cse_object->lt == NULL) cse_object->lt = " ";
    if (cse_object->csi == NULL) cse_object->csi = " ";

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cse_object->ri;
    key_ri.size = strlen(cse_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s",
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

int DB_Store_AE(AE *ae_object) {
    char* DATABASE = "RESOURCE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    char rr[6] ="";

    // if input == NULL
    if (ae_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (ae_object->rn == NULL) ae_object->rn = " ";
    if (ae_object->pi == NULL) ae_object->pi = " ";
    if (ae_object->ty == '\0') ae_object->ty = 0;
    if (ae_object->ct == NULL) ae_object->ct = " ";
    if (ae_object->lt == NULL) ae_object->lt = " ";
    if (ae_object->et == NULL) ae_object->et = " ";

    if(ae_object->rr == false) strcpy(rr,"false");
    else strcpy(rr,"true");

    if (ae_object->api == NULL) ae_object->api = " ";
    if (ae_object->aei == NULL) ae_object->aei = " ";

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = ae_object->ri;
    key_ri.size = strlen(ae_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%s",
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

int DB_Store_CNT(CNT *cnt_object) {
    char* DATABASE = "RESOURCE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    
    // if input == NULL
    if (cnt_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (cnt_object->rn == NULL) cnt_object->rn = " ";
    if (cnt_object->pi == NULL) cnt_object->pi = " ";
    if (cnt_object->ty == '\0') cnt_object->ty = 0;
    if (cnt_object->ct == NULL) cnt_object->ct = " ";
    if (cnt_object->lt == NULL) cnt_object->lt = " ";
    if (cnt_object->et == NULL) cnt_object->et = " ";
    
    if (cnt_object->lbl == NULL) cnt_object->lbl = " ";
    if (cnt_object->acpi == NULL) cnt_object->acpi = " ";
    if (cnt_object->cni == '\0') cnt_object->cni = 0;
    if (cnt_object->cbs == '\0') cnt_object->cbs = 0;
    if (cnt_object->st == '\0') cnt_object->st = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cnt_object->ri;
    key_ri.size = strlen(cnt_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%d;%d;%d",
            cnt_object->rn,cnt_object->pi,cnt_object->ty,cnt_object->ct,cnt_object->lt,cnt_object->et,
            cnt_object->lbl,cnt_object->acpi,cnt_object->cbs,cnt_object->cni,cnt_object->st);

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

int DB_Store_CIN(CIN *cin_object) {
    char* DATABASE = "RESOURCE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data
    
    // if input == NULL
    if (cin_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return -1;
    }
    if (cin_object->rn == NULL) cin_object->rn = " ";
    if (cin_object->pi == NULL) cin_object->pi = "NULL";
    if (cin_object->ty == '\0') cin_object->ty = 0;
    if (cin_object->ct == NULL) cin_object->ct = " ";
    if (cin_object->lt == NULL) cin_object->lt = " ";
    if (cin_object->et == NULL) cin_object->et = " ";

    if (cin_object->con == NULL) cin_object->con = " ";
    if (cin_object->cs == '\0') cin_object->cs = 0;
    if (cin_object->st == '\0') cin_object->st = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);
    
    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cin_object->ri;
    key_ri.size = strlen(cin_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%d;%d",
            cin_object->rn,cin_object->pi,cin_object->ty,cin_object->ct,cin_object->lt,cin_object->et,
            cin_object->con,cin_object->cs,cin_object->st);

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

CSE* DB_Get_CSE(char* ri) {
    char* DATABASE = "RESOURCE.db";

    //struct to return
    CSE* new_cse= calloc(1,sizeof(CSE));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cse->ri = calloc(key.size,sizeof(char));
            strcpy(new_cse->ri, key.data);

            char *ptr = strtok((char*)data.data,DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string

                switch (idx) {
                case 0:
                    if(strcmp(ptr," ")==0) new_cse->rn=NULL; //data is NULL
                    else{
                        new_cse->rn = calloc(strlen(ptr),sizeof(char));
                        strcpy(new_cse->rn, ptr);
                    }
                    idx++;
                    break;
                case 1:
                    if(strcmp(ptr," ")==0) new_cse->pi=NULL; //data is NULL
                    else{
                        new_cse->pi = calloc(strlen(ptr),sizeof(char));
                        strcpy(new_cse->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                    if(strcmp(ptr,"0")==0) new_cse->ty=0;
                    else {new_cse->ty = atoi(ptr);}

                    idx++;
                    break;
                case 3:
                    if(strcmp(ptr," ")==0) new_cse->ct=NULL; //data is NULL
                    else{
                        new_cse->ct = calloc(strlen(ptr),sizeof(char));
                        strcpy(new_cse->ct, ptr);
                    }
                    idx++;
                    break;
                case 4:
                    if(strcmp(ptr," ")==0) new_cse->lt=NULL;
                    else{
                    new_cse->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cse->lt, ptr);
                    }
                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_cse->csi=NULL;
                else{
                    new_cse->csi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cse->csi, ptr);
                }
                    idx++;
                    break;             
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ,
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
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

AE* DB_Get_AE(char* ri) {
    char* DATABASE = "RESOURCE.db";

    //struct to return
    AE* new_ae = calloc(1,sizeof(AE));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_ae->ri = calloc(key.size,sizeof(char));
            strcpy(new_ae->ri, key.data);

            char *ptr = strtok((char*)data.data,DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                if(strcmp(ptr," ")==0) new_ae->rn=NULL; //data is NULL
                else{
                    new_ae->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->rn, ptr);
                }
                    idx++;
                    break;
                case 1:
                if(strcmp(ptr," ")==0) new_ae->pi=NULL; //data is NULL
                    else{
                    new_ae->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strcmp(ptr,"0")==0) new_ae->ty=0;
                else new_ae->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                if(strcmp(ptr," ")==0) new_ae->ct=NULL; //data is NULL
                else{
                    new_ae->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->ct, ptr);
                }
                    idx++;
                    break;
                case 4:
                if(strcmp(ptr," ")==0) new_ae->lt=NULL; //data is NULL
                else{                
                    new_ae->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->lt, ptr);
                }
                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_ae->et=NULL; //data is NULL
                else{                
                    new_ae->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->et, ptr);
                }
                    idx++;
                    break;      
                case 6:
                if(strcmp(ptr," ")==0) new_ae->api=NULL; //data is NULL
                else{                
                    new_ae->api = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->api, ptr);
                }
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
                if(strcmp(ptr," ")==0) new_ae->aei=NULL; //data is NULL
                else{                
                    new_ae->aei = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_ae->aei, ptr);
                }
                    idx++;
                    break;                                                                     
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ,
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
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

CNT* DB_Get_CNT(char* ri) {
    char* DATABASE = "RESOURCE.db";

    //struct to return
    CNT* new_cnt = calloc(1,sizeof(CNT));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cnt->ri = calloc(key.size,sizeof(char));
            strcpy(new_cnt->ri, key.data);

            char *ptr = strtok((char*)data.data,DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                if(strcmp(ptr," ")==0) new_cnt->rn=NULL; //data is NULL
                    else{
                    new_cnt->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->rn, ptr);
                    }
                    idx++;
                    break;
                case 1:
                if(strcmp(ptr," ")==0) new_cnt->pi=NULL; //data is NULL
                    else{
                    new_cnt->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strcmp(ptr,"0")==0) new_cnt->ty=0;
                else new_cnt->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                if(strcmp(ptr," ")==0) new_cnt->ct=NULL; //data is NULL
                    else{
                    new_cnt->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->ct, ptr);
                    }
                    idx++;
                    break;
                case 4:
                if(strcmp(ptr," ")==0) new_cnt->lt=NULL; //data is NULL
                    else{
                    new_cnt->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->lt, ptr);
                    }
                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_cnt->et=NULL; //data is NULL
                    else{
                    new_cnt->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->et, ptr);
                    }
                    idx++;
                    break;      
                case 6:
                if(strcmp(ptr," ")==0) new_cnt->lbl=NULL; //data is NULL
                    else{
                    new_cnt->lbl = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->lbl, ptr);
                    }
                    idx++;
                    break;   
                case 7:
                if(strcmp(ptr," ")==0) new_cnt->acpi=NULL; //data is NULL
                    else{
                    new_cnt->acpi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cnt->acpi, ptr);
                    }
                    idx++;
                    break;                                           
                case 8:
                if(strcmp(ptr,"0")==0) new_cnt->cbs=0;
                else new_cnt->cbs = atoi(ptr);

                    idx++;
                    break;     
                case 9:
                if(strcmp(ptr,"0")==0) new_cnt->cni=0;
                else new_cnt->cni = atoi(ptr);

                    idx++;
                    break;    
                case 10:
                if(strcmp(ptr,"0")==0) new_cnt->st=0;
                else new_cnt->st = atoi(ptr);

                    idx++;
                    break;                                                                    
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ;
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
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

CIN* DB_Get_CIN(char* ri) {
    char* DATABASE = "RESOURCE.db";

    //struct to return
    CIN* new_cin = calloc(1,sizeof(CIN));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cin = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cin++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_cin->ri = calloc(key.size,sizeof(char));
            strcpy(new_cin->ri, key.data);

            char *ptr = strtok((char*)data.data, DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:
                if(strcmp(ptr," ")==0) new_cin->rn=NULL; //data is NULL
                    else{
                    new_cin->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->rn, ptr);
                    }
                    idx++;
                    break;
                case 1:
                if(strcmp(ptr," ")==0) new_cin->pi=NULL; //data is NULL
                    else{
                    new_cin->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strcmp(ptr,"0")==0) new_cin->ty=0;
                    else
                    new_cin->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                if(strcmp(ptr," ")==0) new_cin->ct=NULL; //data is NULL
                    else{
                    new_cin->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->ct, ptr);
                    }
                    idx++;
                    break;
                case 4:
                    new_cin->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->lt, ptr);

                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_cin->et=NULL; //data is NULL
                    else{
                    new_cin->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->et, ptr);
                    }
                    idx++;
                    break;      
                case 6:
                if(strcmp(ptr," ")==0) new_cin->con=NULL; //data is NULL
                    else{
                    new_cin->con = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_cin->con, ptr);
                    }
                    idx++;
                    break;       
                case 7:
                if(strcmp(ptr,"0")==0) new_cin->cs=0;
                    else
                    new_cin->cs = atoi(ptr);

                    idx++;
                    break;            
                case 8:
                if(strcmp(ptr,"0")==0) new_cin->st=0;
                    else
                    new_cin->st = atoi(ptr);

                    idx++;
                    break;                                                                              
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ;
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

ACP* DB_Get_ACP(char* ri) {
    char* DATABASE = "ACP.db";

    //struct to return
    ACP* new_acp = calloc(1,sizeof(ACP));

    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;

    int cnt = 0;
    int flag = 0;
    int idx = 0;
    
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        cnt++;
        if (strncmp(key.data, ri, key.size) == 0) {
            flag=1;
            // ri = key
            new_acp->ri =calloc(key.size,sizeof(char));
            strcpy(new_acp->ri, key.data);

            char *ptr = strtok((char*)data.data, DB_SEP);  //split first string
            while (ptr != NULL) { // Split to end of next string
                switch (idx) {
                case 0:if(strcmp(ptr," ")==0) new_acp->rn=NULL; //data is NULL
                else{
                    new_acp->rn = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->rn, ptr);
                    }
                    idx++;
                    break;
                case 1:
                if(strcmp(ptr," ")==0) new_acp->pi=NULL; //data is NULL
                else{
                    new_acp->pi = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->pi, ptr);
                    }
                    idx++;
                    break;
                case 2:
                if(strcmp(ptr,"0")==0) new_acp->ty=0;
                else new_acp->ty = atoi(ptr);

                    idx++;
                    break;
                case 3:
                if(strcmp(ptr," ")==0) new_acp->ct=NULL; //data is NULL
                else{
                    new_acp->ct = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->ct, ptr);
                    }
                    idx++;
                    break;
                case 4:
                if(strcmp(ptr," ")==0) new_acp->lt=NULL; //data is NULL
                else{
                    new_acp->lt = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->lt, ptr);
                    }
                    idx++;
                    break;                
                case 5:
                if(strcmp(ptr," ")==0) new_acp->et=NULL; //data is NULL
                else{
                    new_acp->et = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->et, ptr);
                }
                    idx++;
                    break;
                case 6:
                if(strcmp(ptr," ")==0) new_acp->pv_acor=NULL; //data is NULL
                else{
                    new_acp->pv_acor = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->pv_acor, ptr);
                }
                    idx++;
                    break;
                case 7:
                if(strcmp(ptr," ")==0) new_acp->pv_acop=NULL; //data is NULL
                else{
                    new_acp->pv_acop = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->pv_acop, ptr);
                }
                    idx++;
                    break;
                case 8:
                if(strcmp(ptr," ")==0) new_acp->pvs_acor=NULL; //data is NULL
                else{
                    new_acp->pvs_acor = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->pvs_acor, ptr);
                }
                    idx++;
                    break;
                case 9:
                if(strcmp(ptr," ")==0) new_acp->pvs_acop=NULL; //data is NULL
                else{
                    new_acp->pvs_acop = calloc(strlen(ptr),sizeof(char));
                    strcpy(new_acp->pvs_acop, ptr);
                }
                    idx++;
                    break;                
                default:
                    idx=-1;
                }
                
                ptr = strtok(NULL, DB_SEP); //The delimiter is ;
            }
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
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

    return new_acp;
}

int DB_Update_AE(AE* ae_object) {
    char* DATABASE = "RESOURCE.db";    
    DB* dbp;
    DBC* dbcp;
    DBT key_ri, data;
    int ret;
    char rr[6]="";

    /* ri NULL ERROR*/
    if(ae_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return -1;
    }

    //Struct to store in DB
    AE* ae = calloc(1,sizeof(AE));
    ae = DB_Get_AE(ae_object->ri);

    if(ae_object->rn!=NULL) strcpy(ae->rn,ae_object->rn);
    if(ae_object->pi!=NULL) strcpy(ae->pi,ae_object->pi);
    if(ae_object->ct!=NULL) strcpy(ae->ct,ae_object->ct);
    if(ae_object->lt!=NULL) strcpy(ae->lt,ae_object->lt);
    if(ae_object->et!=NULL) strcpy(ae->et,ae_object->et);
    if(ae_object->api!=NULL) strcpy(ae->api,ae_object->api);
    if(ae_object->aei!=NULL) strcpy(ae->aei,ae_object->aei);
    if(ae_object->rr != ae->rr){
        if(ae_object->rr == false || ae_object->rr == 0 || ae_object->rr == FALSE) strcpy(rr,"false");
        else strcpy(rr,"true");
    }
    if(ae_object->ty!=0) ae->ty=ae_object->ty;
   
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = ae_object->ri;
    key_ri.size = strlen(ae_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%s",
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
    free(ae);

    return 1;
}

int DB_Update_CNT(CNT* cnt_object) {
    char* DATABASE = "RESOURCE.db";   
    DB* dbp;
    DBC* dbcp;
    DBT key_ri, data;
    int ret;

    /* ri NULL ERROR*/
    if(cnt_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return -1;
    }

    //Struct to store in DB
    CNT* cnt = calloc(1,sizeof(CNT));
    cnt = DB_Get_CNT(cnt_object->ri);

    if(cnt_object->rn!=NULL) strcpy(cnt->rn,cnt_object->rn);
    if(cnt_object->pi!=NULL) strcpy(cnt->pi,cnt_object->pi);
    if(cnt_object->ct!=NULL) strcpy(cnt->ct,cnt_object->ct);
    if(cnt_object->lt!=NULL) strcpy(cnt->lt,cnt_object->lt);
    if(cnt_object->et!=NULL) strcpy(cnt->et,cnt_object->et);
    if(cnt_object->lbl!=NULL) strcpy(cnt->lbl,cnt_object->lbl);
    if(cnt_object->acpi!=NULL) strcpy(cnt->acpi,cnt_object->acpi);    
    if(cnt_object->ty!=0) cnt->ty=cnt_object->ty;
    if(cnt_object->cbs!=0) cnt->cbs=cnt_object->cbs;
    if(cnt_object->cni!=0) cnt->cni=cnt_object->cni;       
    if(cnt_object->st!=0) cnt->st=cnt_object->st;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cnt_object->ri;
    key_ri.size = strlen(cnt_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%d;%d;%d",
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
    free(cnt);

    return 1;
}

int DB_Update_ACP(ACP* acp_object) {
    char* DATABASE = "ACP.db";    
    DB* dbp;
    DBC* dbcp;
    DBT key_ri, data;
    int ret;

    /* ri NULL ERROR*/
    if(acp_object->ri==NULL){
        fprintf(stderr,"ri NULL ERROR\n");
        return -1;
    }

    //Struct to store in DB
    ACP* acp = calloc(1,sizeof(ACP));
    acp = DB_Get_ACP(acp_object->ri);

    if(acp_object->rn!=NULL) strcpy(acp->rn,acp_object->rn);
    if(acp_object->pi!=NULL) strcpy(acp->pi,acp_object->pi);
    if(acp_object->ct!=NULL) strcpy(acp->ct,acp_object->ct);
    if(acp_object->lt!=NULL) strcpy(acp->lt,acp_object->lt);
    if(acp_object->et!=NULL) strcpy(acp->et,acp_object->et);
    if(acp_object->pv_acor!=NULL) strcpy(acp->pv_acor,acp_object->pv_acor);
    if(acp_object->pv_acop!=NULL) strcpy(acp->pv_acop,acp_object->pv_acop);
    if(acp_object->pvs_acor!=NULL) strcpy(acp->pvs_acor,acp_object->pvs_acor);       
    if(acp_object->pvs_acop!=NULL) strcpy(acp->pvs_acop,acp_object->pvs_acop);    
    if(acp_object->ty!=0) acp->ty=acp_object->ty;
   
    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* key and data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = acp_object->ri;
    key_ri.size = strlen(acp_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s;%s;%d;%s;%s;%s;%s;%s;%s;%s",
            acp->rn,acp->pi,acp->ty,acp->ct,acp->lt,
            acp->et,acp->pv_acor,acp->pv_acop,acp->pvs_acor,acp->pvs_acop);

    data.data = str;
    data.size = strlen(str) + 1;
    
    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_ri, &data, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    /* DB close */
    dbcp->close(dbcp);
    dbp->close(dbp, 0);
    free(acp);

    return 1;
}

int DB_Delete(char* ri) {
    char* DATABASE = "RESOURCE.db";
    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;
    int flag = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, ri, key.size) == 0) {
            flag = 1;
            dbcp->del(dbcp, 0);
            break;
        }
    }
    if (flag == 0) {
        fprintf(stderr, "Not Found\n");
        return -1;
    }

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);
    
    /* Delete Success */
    return 1;
}

int DB_Delete_ACP(char* ri) {
    char* DATABASE = "ACP.db";
    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int ret;
    int flag = 0;

    dbp = DB_CREATE_(dbp);
    dbp = DB_OPEN_(dbp,DATABASE);
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, ri, key.size) == 0) {
            flag = 1;
            dbcp->del(dbcp, 0);
            break;
        }
    }
    if (flag == 0) {
        fprintf(stderr, "Not Found\n");
        return -1;
    }

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);
    
    /* Delete Success */
    return 1;
}

Node* DB_Get_All_CSE() {
    char* DATABASE = "RESOURCE.db";
    char* TYPE = "5-";

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

    int cse = 0;
    DBC* dbcp0;
    dbcp0 = DB_GET_CURSOR(dbp,dbcp0);
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0) 
            cse++;
    }
    //fprintf(stderr, "<%d>\n",cse);

    if (cse == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    Node* head = calloc(cse,sizeof(Node));
    Node* node;
    node = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0){
            CSE* cse = DB_Get_CSE((char*)key.data);
            node->ri = calloc(strlen(cse->ri)+1,sizeof(char));
            node->rn = calloc(strlen(cse->rn)+1,sizeof(char));
            node->pi = calloc(strlen(cse->pi)+1,sizeof(char));

            strcpy(node->ri,cse->ri);
            strcpy(node->rn,cse->rn);
            strcpy(node->pi,cse->pi);
            node->ty = cse->ty;

            node->siblingRight=calloc(1,sizeof(Node));            
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;
            free(cse);
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
    free(node);
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
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
            //node->ty = calloc(sizeof(ae->ty),sizeof(int));

            strcpy(node->ri,ae->ri);
            strcpy(node->rn,ae->rn);
            strcpy(node->pi,ae->pi);
            node->ty = ae->ty;

            node->siblingRight=(Node*)calloc(1,sizeof(Node));
            //node->siblingRight=calloc(1,sizeof(Node));            
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
    free(node->ri);
    free(node->rn);
    free(node->pi);    
    free(node);
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
}
Node* DB_Get_All_CNT() {
    char* DATABASE = "RESOURCE.db";
    char* TYPE = "3-";

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
            CNT* CNT = DB_Get_CNT((char*)key.data);
            node->ri = calloc(strlen(CNT->ri)+1,sizeof(char));
            node->rn = calloc(strlen(CNT->rn)+1,sizeof(char));
            node->pi = calloc(strlen(CNT->pi)+1,sizeof(char));

            strcpy(node->ri,CNT->ri);
            strcpy(node->rn,CNT->rn);
            strcpy(node->pi,CNT->pi);
            node->ty = CNT->ty;

            node->siblingRight=calloc(1,sizeof(Node));            
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;
            free(CNT);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        fprintf(stderr, "Cursor ERROR\n");
        return NULL;
    }

    node->siblingLeft->siblingRight = NULL;
    free(node);
    free(node->ri);
    free(node->pi);
    free(node->rn);            
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
}

Node* DB_Get_All_ACP() {
    char* DATABASE = "ACP.db";
    char* TYPE = "1-";

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

    int acp = 0;
    DBC* dbcp0;
    dbcp0 = DB_GET_CURSOR(dbp,dbcp0);
    while ((ret = dbcp0->get(dbcp0, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0) 
            acp++;
    }
    //fprintf(stderr, "<%d>\n",acp);

    if (acp == 0) {
        fprintf(stderr, "Data not exist\n");
        return NULL;
    }

    Node* head = calloc(acp,sizeof(Node));
    Node* node;
    node = head;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, TYPE , 2) == 0){
            ACP* acp = DB_Get_ACP((char*)key.data);
            node->ri = calloc(strlen(acp->ri)+1,sizeof(char));
            node->rn = calloc(strlen(acp->rn)+1,sizeof(char));
            node->pi = calloc(strlen(acp->pi)+1,sizeof(char));

            strcpy(node->ri,acp->ri);
            strcpy(node->rn,acp->rn);
            strcpy(node->pi,acp->pi);
            node->ty = acp->ty;

            node->siblingRight=calloc(1,sizeof(Node));            
            node->siblingRight->siblingLeft = node;
            node = node->siblingRight;
            free(acp);
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
    free(node);
    node = NULL;

    /* Cursors must be closed */
    if (dbcp != NULL)
        dbcp->close(dbcp);
    if (dbp != NULL)
        dbp->close(dbp, 0);    

    return head;
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



/*
 Function name: Store_Label
 Input: string label, string URI
 Return: 1 or -1
 Description:
 Function to store Label as Key and URI as value in LABEL.db
*/
int Store_Label(char* label, char* uri) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_label;
    DBT data_uri;  // storving key and real data

    // if input is NULL => return -1
    if (label == NULL) {
        fprintf(stderr, "Label is empty\n");
        return -1;
    }
    if (uri == NULL) {
        fprintf(stderr, "URI is empty\n");
        return -1;
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* keyand data must initialize */
    memset(&key_label, 0, sizeof(DBT));
    memset(&data_uri, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_label.data = label;
    key_label.size = strlen(label) + 1;
    data_uri.data = uri;
    data_uri.size = strlen(uri) + 1;

    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_label, &data_uri, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close

    return 1;
}

/*
 Function name: Label_To_URI
 Input: string label
 Return: string uri
 Description:
 It receives the Label value as an argument from the <key: Label, value:URI> type DB 
 and returns the URI.
*/
char* Label_To_URI(char* label) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    // if input is NULL => return -1
    if (label == NULL) {
        fprintf(stderr, "Label is empty\n");
        exit(-1);
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* 0 if LABEL does not exist, 1 if present */
    int flag = 0;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, label, key.size) == 0) {
            flag=1;
            return (char*)data.data;

            //DB close
            dbcp->close(dbcp);
            dbp->close(dbp, 0); 
            break;
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        return NULL;
    }
    if (flag == 0)
        return NULL;

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close
}

/*
 Function name: URI_To_Label
 Input: string URI
 Return: string label
 Description:
 It receives the URI value as an argument from the <key: Label, value:URI> type DB
 and returns the label.
*/
char* URI_To_Label(char* uri) {
    char* DATABASE_label = "LABEL.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    DBT key, data;  // storving key and real data
    int ret;        // template value

    // if input is NULL => return -1
    if (uri == NULL) {
        fprintf(stderr, "URI is empty\n");
        exit(-1);
    }

    dbp = DB_CREATE_(dbp);
    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE_label, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE_label);
        printf("DB Open ERROR\n");
        exit(1);
    }
    dbcp = DB_GET_CURSOR(dbp,dbcp);

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* 0 if LABEL does not exist, 1 if present */
    int flag = 0;

    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(data.data, uri, data.size) == 0) {
            flag = 1;
            return (char*)key.data;

            //DB close
            dbcp->close(dbcp);
            dbp->close(dbp, 0);
            break;
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        return NULL;
    }
    if (flag == 0)
        return NULL;

    dbcp->close(dbcp);
    dbp->close(dbp, 0); //DB close
}