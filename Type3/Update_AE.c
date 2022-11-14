#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include "onem2m.h"

int main() {

    AE ae_after;
    ae_after.rn = "Sensor2_updateeee";
    ae_after.ri = "TAE2";
    ae_after.rr = true; 
    ae_after.api = "tinyProject2_update";

    int flag = DB_Update_AE(&ae_after);
    if(flag==1)
        display("RESOURCE.db");

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
        exit(0);
    }
    return dbcp;
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
        if(ae_object->rr == false) strcpy(rr,"false");
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