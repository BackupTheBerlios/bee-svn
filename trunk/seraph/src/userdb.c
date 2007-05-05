#include "config.h"
#include "dbg.h"
#include "basedb.h"
#include "baseclass.h"
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


/* Avoid directory traversal */
static const char*
userdb_stripTraversal( const char* const uname)
{
    char *it = uname + strlen(uname) ;
    while( --it > uname )
    {   if( *it == '/' || *it == '\\' )
        {   it++;
            break;
        }
    }
    debug("it[%s]\n", it);
    return it;
}

bool
userdb_register( const char* const name, const char* const email,
                 const char* const uname, const char* const pass)
{
    const char* it = userdb_stripTraversal(uname);

    if( chdir(USERDB) )
    {   debug("Unable to chdir to %s\n", USERDB, strerror(errno));
        return false;
    }
    if( mkdir(it, 0755) || chdir(it) )
    {   debug("Unable to register user: [%s]\n", strerror(errno) );
        return false;
    }

    if( fclose(fopen("userdata","w")) )
    {   debug("Unable to create 'userdata' file: [%s]\n", strerror(errno));
        return false;
    }
    struct Class* db = (struct Class*)new(BaseDB);
    /*TODO: check the retval of db_open */
    db_open( db, "%s/%s/%s", USERDB, it,"userdata");
    /* TODO: use exceptions instead of || */
    if( !db_put(db, "name", name)
    ||  !db_put(db, "email", email)
    ||  !db_put(db, "uname", it)
    ||  !db_put(db, "password", pass)
    ||  !db_close(db))
        return false;
    delete(db);
    if( !mkJobDir(uname, "jobs")
    ||  !mkJobDir(uname, "jobs/complete")
    ||  !mkJobDir(uname, "jobs/running")
    ||  !mkJobDir(uname, "jobs/pending"))
        return false;
    return true;
}

bool
userdb_login( const char* uname, const char* pass)
{
    char p[33]={0}; //md5sum
    const char* it = userdb_stripTraversal(uname);

    struct Class* db = new(BaseDB);
    if( db_open(db, "%s/%s/%s", USERDB, it, "userdata") )
        return false;

    if( !db_get(db, "pass", p)
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
    char    *rb=NULL, *path=NULL;
    const char* it= userdb_stripTraversal(uname);

    path = (char*)malloc( strlen(USERDB) + strlen(it) + strlen(log) + 16);
    if( !path )
    {   *ret = NULL;
        return 0;
    }

    sprintf( path, "%s/%s/jobs/running/%s", USERDB, it, log);
    printf("Encoding [%s]\n", path);
    fd = open(path, O_RDONLY);
    free(path);
    if( fd < 0 )
    {   *ret = NULL;
        return 0;
    }

    if( fstat(fd, &s) )
    {   printf("Error fstating\n");
        *ret = NULL;
        close(fd);
        return 0;
    }
    rb = (char*)malloc(s.st_size);
    printf("filesize [%d]\n", s.st_size);
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
    const char* it = userdb_stripTraversal(uname);

    *jobs =  g_slist_alloc() ;

    debug("JobType: [%d]\n", job_type);
    if( job_type == JOB_ALL )
        job_type += JOB_PENDING + JOB_RUNNING + JOB_COMPLETE;

    if( job_type & JOB_PENDING )
    {
        sprintf( path, "%s/%s/jobs/pending", USERDB, it);
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
    if( job_type & JOB_RUNNING )
    {
        struct Class* db = new(BaseDB);
        char *data=malloc(34);

        sprintf( path, "%s/%s/jobs/running", USERDB, it);
        debug("JOBS_RUNNING path[%s]\n", path);
        dit  = opendir(path);
        if(!dit)
        {   free(data);
            delete(db);
            return false;
        }

        while( dent=readdir(dit) )
        {
            if( !strcmp(dent->d_name, ".") || !strcmp(dent->d_name,"..") ) continue;
            debug("job_append[%s]\n", dent->d_name);
            /* TODO: Check retval*/
            db_open(db, "%s/%s/jobs/running/%s", USERDB, it, dent->d_name);
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
        sprintf( path, "%s/%s/jobs/complete", USERDB, it);
        debug("JOBS_COMPLETE path[%s]\n", path);
        dit = opendir(path);
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
    char *it=NULL;
    struct Class* db = new(BaseDB);

    debug("uname[%s] session[%s] ip[%s]\n", uname, session, ip);
    it = userdb_stripTraversal(uname);
    db_open( db, "%s/%s/%s", USERDB, it, "userdata");
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
    char u[33]={0}, p[33]={0}, *it;
    struct Class* db = new(BaseDB);
    int ret=1;

    debug("uname[%s] password[%s]\n", uname, password);
    it = userdb_stripTraversal(uname);
    ret = db_open( db, "%s/%s/%s", USERDB, it, "userdata");
    debug("USERDB[%s] it[%s]\n", USERDB, it);
    if(ret)
    {
        debug("error opening userdata file for [%s]\n", it);
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
