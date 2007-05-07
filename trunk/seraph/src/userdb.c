#include "config.h"
#include "dbg.h"
#include "baseclass.h"
#include "basedb.h"
#include "userdb.h"
static bool
mkJobDir( const char* const uname, const char* const jobType)
{
    char *path=NULL;
    path=(char*) malloc(strlen(USERDB)+strlen(uname)+strlen(jobType)+8);
    if(!path) return false;

    sprintf( path, "%s/%s/%s", USERDB, uname, jobType);
    if( mkdir(path, 0755) )
    {
        debug("unable to register user: [%s]\n", strerror(errno) );
        free(path);
        return false;
    }
    debug("mkdir(%s)\n", path);
    free(path);
    return true;
}

bool
userdb_register( const char* const name, const char* const email,
                 const char* const uname, const char* const pass)
{
    if(chdir(USERDB))
    {
        debug("unable to chdir to %s\n", USERDB, strerror(errno));
        return false;
    }
    if( mkdir(uname, 0755) || chdir(uname))
    {   debug("unable to register user: [%s]\n", strerror(errno) );
        return false;
    }

    if(fclose(fopen("userdata","w")))
    {   debug("unable to create 'userdata' file: [%s]\n", strerror(errno));
        return false;
    }
    struct Class* db = (struct Class*)new(BaseDB);
    db_open( db, "%s/%s/%s", USERDB,uname,"userdata");/*TODO: check retval*/
    if( !db_put(db, "name", name)
    ||  !db_put(db, "email", email)
    ||  !db_put(db, "uname", uname)
    ||  !db_put(db, "password", pass)
    ||  !db_close(db))
        return false;   /* TODO: use exceptions instead of || */
    delete(db);
    /* TODO check return values */
    if( !mkJobDir(uname, "jobs")
    ||  !mkJobDir(uname, "jobs/complete")
    ||  !mkJobDir(uname, "jobs/running")
    ||  !mkJobDir(uname, "jobs/pending")
    )
        return false;
    return true;
}

bool
userdb_login( const char* uname, const char* pass)
{
    char p[33]={0}; //md5sum

    struct Class* db = new(BaseDB);
    if( db_open(db, "%s/%s/%s", USERDB, uname, "userdata") )
        return false;

    if( !db_get(db, "pass", &p)
    ||  !db_close(db) )
        return false;
    return !( strcmp(p, pass) );
}

int
userdb_addJob( int job_type )
{
    return 0;
}


size_t
userdb_getErrorLog( const char * uname, int job_type, const char*const log, char* *ret)
{
    struct  stat s;
    int     fd=-1, len=0;
    char    *rb=NULL;
    char    path[PATH_MAX]={0}; /*TODO: use malloc*/

    sprintf( path, "%s/%s/jobs/running/%s", USERDB, uname, log);
    debug("encoding [%s]\n", path);
    fd = open(path, O_RDONLY);
    if( fd <0) return 0;

    if(fstat( fd, &s))
    {   printf("error fstating\n");
        *ret = NULL;
        close(fd);
        return 0;
    }
    rb = (char*)malloc(s.st_size+1);
    debug("filesize [%d]\n", s.st_size);
    len = read( fd, rb, s.st_size);
    rb[len]='\0';
    debug("read from log [%s]\n", rb);
    *ret = rb;
    close(fd);
    return s.st_size;
}

int
userdb_listJobs( const char * const uname, enum JobType job_type, GSList** jobs)
{
    int nbJobs=0;
    DIR* dit;
    struct dirent* dent;
    char path[PATH_MAX]={0};
    struct job* job;
    *jobs =  g_slist_alloc() ;

    debug("JobType: [%d]\n", job_type);
    if( job_type == JOB_ALL )
        job_type += JOB_PENDING + JOB_RUNNING + JOB_COMPLETE;

    if( job_type & JOB_PENDING )
    {
        sprintf( path, "%s/%s/jobs/pending", USERDB, uname);
        debug("JOBS_PENDING path[%s]\n", path);

        dit = opendir(path);
        if( !dit )
        {
            return nbJobs;
        }
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
    if( job_type & JOB_RUNNING )
    {
        struct Class* db = new(BaseDB);
        char *data=malloc(34);

        sprintf( path, "%s/%s/jobs/running", USERDB, uname);
        debug("JOBS_RUNNING path[%s]\n", path);
        dit  = opendir(path);
        if( !dit )
        {   free(data);
            delete(db);
            return nbJobs;
        }
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            /* TODO: Check retval*/
            db_open(db, "%s/%s/jobs/running/%s", USERDB, uname, dent->d_name);
            job =(struct job*)malloc(sizeof(struct job));
            if(!job) break ;
            /* Fill in the struct */
            /* TODO: what to do if db_get fails ? */
            job->name = strdup(dent->d_name) ;

            db_get( db, "date", data);
            job->date = strdup(data);

            db_get( db, "time", data);
            job->time = strdup(data);

            db_get( db, "tests", data);
            job->tests= strdup(data);

            db_get( db, "ctest", data);
            job->ctest= strdup(data);

            *jobs = g_slist_append(*jobs, job );
            ++nbJobs;
            db_close(db);
        }
        closedir( dit );
        free(data);
        delete(db);
    }
    if( job_type & JOB_COMPLETE )
    {
        struct job* job=NULL;
        sprintf( path, "%s/%s/jobs/complete", USERDB, uname);
        debug("JOBS_COMPLETE path[%s]\n", path);
        dit = opendir(path);
        if( !dit )
        {
            return nbJobs;
        }
        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            *jobs = g_slist_append(*jobs, strdup(dent->d_name) );
            ++nbJobs;
        }
        closedir( dit );
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
    char c[33]={0}, i[33]={0}, s[33]={0};

    ret = db_open( db, "%s/%s/%s",USERDB, uname, "userdata");
    debug("USERDB[%s] uname[%s]\n", USERDB, uname);
    if(ret)
    {
        debug("error opening userdata file for [%s]\n", uname);
        delete(db);
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
    db_open( db, "%s/%s/%s", USERDB, uname, "userdata");
    db_put(db, "session", session);
    db_put(db, "ip", ip);
    db_close(db);
    delete(db);
    return true;
}

bool
userdb_checkLogin(  const char* const uname,
                    const char* const password)
{
    char u[33]={0}, p[33]={0};
    struct Class* db = new(BaseDB);
    int ret=1;

    debug("uname[%s] password[%s]\n", uname, password);
    ret = db_open( db, "%s/%s/%s", USERDB, uname, "userdata");
    debug("USERDB[%s] uname[%s]\n", USERDB, uname);
    if(ret)
    {
        debug("error opening userdata file for [%s]\n", uname);
        delete(db);
        return false;
    }
    db_get(db, "password", &p);
    db_close(db);
    delete(db);
    ret = strcmp(password, p);
    printf("isPasswordOk?[%d]\n", !ret);
    return !( ret );
}

bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie )
{
    char c[33]={0};
    struct Class* db = new(BaseDB);

    debug("uname[%s] cookie[%s]\n", uname, cookie);
    db_open( db, "%s/%s/%s", USERDB, uname,"userdata" );
    db_get( db, "cookie", &c );
    db_close(db);
    delete(db);
    return false; /*TODO*/
}
