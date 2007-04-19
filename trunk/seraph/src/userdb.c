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
    if(! (f=fopen("metadata", "w")) )
    {   debug("unable to create 'metadata' file: [%s]\n", strerror(errno));
        return false;
    }
    fprintf(f, "%s", pass);
    fclose(f);
    return true;
}

bool
userdb_login( const char* uname, const char* pass)
{
    char p[33]={0}; //md5sum
    FILE* f=NULL;
    char path[PATH_MAX]={0};

    sprintf(path, "%s/%s", USERDB, uname);
    if( !access(path, R_OK ) )
        return false;
    if(! (f=fopen(pass, "r")) )
        return false;

    fscanf(f, "%s", p );
    if( strcmp(p, pass) )
        return false;
    return true;
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

    if(job_type & JOB_ALL )
    {
        job_type |= JOB_PENDING;
        job_type |= JOB_RUNNING;
        job_type |= JOB_COMPLETE;
    }
    if(job_type & JOB_PENDING )
    {
        //dit = opendir(USERDB/uname/jobs/pending);
        while( dent=readdir(dit) )
        {
            if( !strstr(dent->d_name,".") || !strstr(dent->d_name, "..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name);
        }
        closedir(dit);
    }
    if(job_type & JOB_RUNNING )
    {
        //dit  = opendir(USERDB/uname/jobs/running);
        while( dent=readdir(dit) )
        {
            if( !strstr(dent->d_name, ".") || !strstr(dent->d_name,"..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name);//TODO: use a copy of dent->d_name
        }
        close( dit );
    }
    if(job_type & JOB_COMPLETE )
    {
        //dit = opendir(USERDB/uname/jobs/complete);
        while( dent=readdir(dit) )
        {
            if( !strstr(dent->d_name, ".") || !strstr(dent->d_name,"..") ) continue;
            ++nbJobs;
            g_slist_append(*jobs, dent->d_name); //TODO: use a copy of dent->d_name
        }
        close( dit );
    }
    return nbJobs;
}





bool
userdb_checkSession(const char* const username,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip )
{
}

bool
userdb_setSession(  const char* const id,
                    const char* const session,
                    const char* const ip){}

bool
userdb_checkLogin(  const char* const username,
                    const char* const password){}

bool
userdb_checkRemembered( const char* const username,
                        const char* const cookie ) {}
