//#include "userdb.h"
#include "config.h"
#include <unistd.h>     //chdir
#include <sys/types.h>  //opendir
#include <dirent.h>     //opendir
#include <glib.h>       //GSList
#include "baseclass.h"
#include "basedb.h"


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

    fclose(fopen("userdata","w"));
    if(fclose(fopen("userdata","w")))
    {   debug("unable to create 'userdata' file: [%s]\n", strerror(errno));
        return false;
    }
    struct Class* db = new(BaseDB);
    db_open( db, "userdata",0);
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
    int ret =0;
    struct Class* db = new(BaseDB);
    char u[33]={0}, c[33]={0}, i[33]={0}, s[33]={0};

    ret = db_open( db, "%s/%s",USERDB, uname);
    if(ret)
    {
        printf("error opening userdata file for [%s]\n", uname);
        return false;
    }

    if( db_get(db, "user",   &u)
    &&  db_get(db, "cookie", &c)
    &&  db_get(db, "ip",     &i)
    &&  db_get(db, "session",&s)
      )
    {
        db_close(db);
        return false;
    }
    db_close(db);
    //TODO: strcmp(uname, u) ...
    return true;
}

bool
userdb_setSession(  const char* const uname,
                    const char* const id,
                    const char* const session,
                    const char* const ip)
{
    struct Class* db = new(BaseDB);
    db_open( db, "%s/%s", USERDB, uname);
    db_put(db, "session", session);
    db_put(db, "ip", ip);
    db_close(db);
}

bool
userdb_checkLogin(  const char* const uname,
                    const char* const password)
{
    char u[33]={0}, p[33]={0};
    struct Class* db = new(BaseDB);
    db_open( db, "%s/%s", USERDB, uname);
    db_get(db, "user", &u);
    db_get(db, "pass", &p);
    db_close(db);
    return !( strcmp(uname, u) && strcmp(password, p) );

}

bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie )
{
    char c[33]={0};
    struct Class* db = new(BaseDB);
    new(BaseDB);
    db_open( db, "%s/%s", USERDB, uname );
    db_get( db, "cookie", &c );
    db_close(db);
}
