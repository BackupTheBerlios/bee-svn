//#include "userdb.h"
#include "config.h"
#include <unistd.h>     //chdir
#include <sys/types.h>  //opendir
#include <dirent.h>     //opendir
#include <glib.h>       //GSList


enum JobType {
    JOB_PENDING,
    JOB_RUNNING,
    JOB_COMPLETE,
    JOB_ALL
};

bool
userdb_register( const char* const name, const char* const email,
                 const char* const uname, const char* const pass)
{
    FILE* f=NULL;
    if(chdir(USERDB))
    {
        debug("unable to chdir to %s\n", USERDB, strerror(errno));
        return false;
    }
    if( mkdir(uname, 0755) )
    {
        debug("unable to register user: [%s]\n", strerror(errno) );
        return false;
    }
    chdir(uname);
    new(BaseDB);
    if(db_open("userdata", DB_CREATE))
    {   debug("unable to create 'userdata' file: [%s]\n", strerror(errno));
        return false;
    }
    db_put(dbp, "name", name);
    db_put(dbp, "email", email);
    db_put(dbp, "uname", uname);
    db_put(dbp, "pass", pass);
    db_close(dbp);
    return true;
}

bool
userdb_login( const char* uname, const char* pass)
{
    char* p=NULL; //md5sum

    new(BaseDB);
    if(db_open(dbp, DB_OPEN, "%s/%s", USERDB, uname))
        return false;

    db_get(dbp, "pass", &p);
    db_close( dbp);
    return !( strcmp(p, pass) );
}

int
userdb_addJob( int job_type )
{
    return 0;
}


int
userdb_listJobs( const char * const uname, enum JobType job_type, GSList** jobs)
{
    int nbJobs=0;
    DIR* dit;
    struct dirent* dent;
    char path[PATH_MAX]={0};

    if(job_type & JOB_ALL )
    {
        job_type |= JOB_PENDING;
        job_type |= JOB_RUNNING;
        job_type |= JOB_COMPLETE;
    }
    if(job_type & JOB_PENDING )
    {
        sprintf( path, "%s/%s/jobs/pending", USERDB, uname);
        dit = opendir(path);
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name,".") || !strcmp(dent->d_name, "..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name);
        }
        closedir(dit);
    }
    if(job_type & JOB_RUNNING )
    {
        sprintf( path, "%s/%s/jobs/running", USERDB, uname);
        dit  = opendir(path);
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name);//TODO: use a copy of dent->d_name
        }
        close( dit );
    }
    if(job_type & JOB_COMPLETE )
    {
        sprintf( path, "%s/%s/jobs/complete", USERDB, uname);
        dit = opendir(path);
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name); //TODO: use a copy of dent->d_name
        }
        close( dit );
    }
    return nbJobs;
}





bool
userdb_checkSession(const char* const uname,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip )
{
    FILE *f=NULL;
    int ret =0;
    char *

    new(BaseDB);
    ret = db_open( &dbp, DB_OPEN, "%s/%s/userdata",USERDB, uname);
    if(ret)
    {
        printf("error opening userdata file for [%s]\n", uname);
        return false;
    }

    if( db_get(dbp, "user",   &user)
    &&  db_get(dbp, "cookie", &cookie)
    &&  db_get(dbp, "ip",     &ip)
    &&  db_get(dbp, "session",&session)
      )
    {
        db_close(dbp);
        return false;
    }
    db_close(dbp);
    return true;
}

bool
userdb_setSession(  const char* const id,
                    const char* const session,
                    const char* const ip)
{
    new(BaseDB);
    db_open( &dbp, DB_OPEN, "%s/%s", USERDB, uname);
    db_put(dbp, "session", session);
    db_put(dbp, "ip", ip);
    db_close(dbp);
}

bool
userdb_checkLogin(  const char* const uname,
                    const char* const password)
{
    char *u=NULL, *p=NULL;
    new(BaseDB);
    db_open( &dbp, DB_OPEN, "%s/%s", USERDB, uname);
    db_get(dbp, "user", &u);
    db_get(dbp, "pass", &p);
    db_close(dbp);
    return !( strcmp(uname, u) && strcmp(password, p) );

}

bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie )
{
    char *c;
    new(BaseDB);
    db_open( &dbp, DB_OPEN, "%s/%s", USERDB, uname );
    db_get( dbp, "cookie", &c );
    db_close(dbp);
}
