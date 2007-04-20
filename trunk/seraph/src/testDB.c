#include <db.h>

int main()
{
    DB_ENV* myEnv=NULL;
    DB* dbp=NULL;
    int db_flags;
    int env_flags;
    int ret;

    ret = db_env_create(&myEnv, 0);
    if(ret) {
        printf("error creating environ\n");
        return 0;
    }
    env_flags = DB_CREATE | DB_INIT_MPOOL;
    ret = myEnv->open(myEnv, "/tmp", env_flags, 0);
    if(ret) {
        printf("Error in opening env");
        return 0;
    }



    ret = db_create(&dbp, myEnv, 0);
    if(ret)
    {
        printf("ERROR CREATING DATABASE\n");
        return 0;
    }
    db_flags = DB_CREATE ;

    ret = dbp->open(dbp, NULL, "users.db", NULL, DB_BTREE, db_flags, 0);
    if(ret)
    {
        printf("error opening database\n");
        return 0;
    }
    dbp->close(dbp,0);
    myEnv->close(myEnv, 0);
    return 0;
}
