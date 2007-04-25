//#include "userdb.h"
#include "config.h"
#include <unistd.h>     //chdir
#include <sys/types.h>  //opendir
#include <dirent.h>     //opendir
#include <glib.h>       //GSList
#include "baseclass.h"
#include "basedb.h"
#include "userdb.h"


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

    fclose(fopen("userdata","w"));
    if(fclose(fopen("userdata","w")))
    {   debug("unable to create 'userdata' file: [%s]\n", strerror(errno));
        return false;
    }
    struct Class* db = new(BaseDB);
    db_open( db, "%s/%s", USERDB,uname);
    db_put(db, "name", name);
    db_put(db, "email", email);
    db_put(db, "uname", uname);
    db_put(db, "pass", pass);
    db_close(db);
    return true;
}

bool
userdb_login( const char* uname, const char* pass)
{
    char p[33]={0}; //md5sum

    struct Class* db = new(BaseDB);
    if(db_open(db, "%s/%s", USERDB, uname))
        return false;

    db_get(db, "pass", p);
    db_close( db);
    return !( strcmp(p, pass) );
}

int
userdb_addJob( int job_type )
{
    return 0;
}


char*
userdb_getErrorLog( const char * uname, int job_type, const char*const log)
{
    struct  stat s;
    int     fd=-1;
    char    *rb=NULL;
    char    path[PATH_MAX]={0};
    buffer_st b64;
    char    *ret=NULL;

    sprintf( path, "%s/%s/jobs/running/%s", USERDB, user, log);
    fd = open(path,"r");
    if( fd <0)
        return NULL;
    fstat( fd, &s);
    rb = (char*)malloc(s.st_size);
    read( fd, rb, s.st_size);
    base64_encode( &b64, rb, s.st_size);
    free(rb);
    close(fd);
    ret = strdup(b64.data);
    buffer_delete(&b64);
    return ret;
}

int
userdb_listJobs( const char * const uname, enum JobType job_type, GSList** jobs)
{
    int nbJobs=0;
    DIR* dit;
    struct dirent* dent;
    char path[PATH_MAX]={0};
    *jobs =  g_slist_alloc() ;

    debug("JobType: [%d]\n", job_type);
    if(job_type & JOB_ALL )
    {
        job_type |= JOB_PENDING;
        job_type |= JOB_RUNNING;
        job_type |= JOB_COMPLETE;
    }
    if(job_type & JOB_PENDING )
    {
        sprintf( path, "%s/%s/jobs/pending", USERDB, uname);
        debug("JOBS_PENDING path[%s]\n", path);

        dit = opendir(path);
        while( (dent=readdir(dit)) != NULL )
        {
            if( !strcmp(dent->d_name,".") || !strcmp(dent->d_name, "..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            *jobs = g_slist_append(*jobs, strdup(dent->d_name) );
            ++nbJobs;
            debug("jobsInQueue[%d]\n", nbJobs);
        }
        closedir(dit);
    }
    if(job_type & JOB_RUNNING )
    {
        sprintf( path, "%s/%s/jobs/running", USERDB, uname);
        debug("JOBS_RUNNING path[%s]\n", path);
        dit  = opendir(path);
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            *jobs = g_slist_append(*jobs, strdup(dent->d_name) );
            ++nbJobs;
        }
        close( dit );
    }
    if(job_type & JOB_COMPLETE )
    {
        sprintf( path, "%s/%s/jobs/complete", USERDB, uname);
        debug("JOBS_COMPLETE path[%s]\n", path);
        dit = opendir(path);
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            *jobs = g_slist_append(*jobs, strdup(dent->d_name) );
            ++nbJobs;
        }
        close( dit );
    }
    debug("return nbJobs[%d]\n", nbJobs);
    return nbJobs;
}





bool
userdb_checkSession(const char* const uname,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip )
{
    int ret =0;
    struct Class* db = new(BaseDB);
    char u[33]={0}, c[33]={0}, i[33]={0}, s[33]={0};

    ret = db_open( db, "%s/%s",USERDB, uname);
    debug("USERDB[%s] uname[%s]\n", USERDB, uname);
    if(ret)
    {
        printf("error opening userdata file for [%s]\n", uname);
        return false;
    }

    if( !db_get(db, "cookie", &c)
    &&  !db_get(db, "ip",     &i)
    &&  !db_get(db, "session",&s)
      )
    {
        db_close(db);
        delete(db);
        return false;
    }
    db_close(db);
    delete(db);
    //TODO: strcmp(uname, u) ...
    return true;
}

bool
userdb_setSession(  const char* const uname,
                    const char* const session,
                    const char* const ip)
{
    struct Class* db = new(BaseDB);

    debug("uname[%s] session[%s] ip[%s]\n", uname, session, ip);
    db_open( db, "%s/%s", USERDB, uname);
    db_put(db, "session", session);
    db_put(db, "ip", ip);
    db_close(db);
    delete(db);
}

bool
userdb_checkLogin(  const char* const uname,
                    const char* const password)
{
    char u[33]={0}, p[33]={0};
    struct Class* db = new(BaseDB);

    debug("uname[%s] password[%s]\n", uname, password);
    db_open( db, "%s/%s", USERDB, uname);
    db_get(db, "pass", &p);
    db_close(db);
    return !( strcmp(password, p) );
}

bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie )
{
    char c[33]={0};
    struct Class* db = new(BaseDB);

    debug("uname[%s] cookie[%s]\n", uname, cookie);
    new(BaseDB);
    db_open( db, "%s/%s", USERDB, uname );
    db_get( db, "cookie", &c );
    db_close(db);
}
