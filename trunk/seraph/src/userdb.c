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
            if( !strstr(dent->d_name,".") || !strstr(dent->d_name, "..") ) continue;
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
            if( !strstr(dent->d_name, ".") || !strstr(dent->d_name,"..") ) continue;
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
            if( !strstr(dent->d_name, ".") || !strstr(dent->d_name,"..") ) continue;
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
    char path[PATH_MAX] = {0};
    char _uname[40]={0}, _cookie[40]={0}, _session[40]={0}, _ip[40]={0};
    FILE *f=NULL;

    sprintf( path, "%s/%s/userdata", USERDB, uname);
    if( !(f=fopen( path, "r")) )
    {
        printf("error opening userdata file for [%s]\n", uname);
        return false;
    }
    fscanf( f, "%s:%s:%s:%s", _uname, _cookie, _session, _ip);
    if( strstr(_uname, uname) && strstr(_cookie, cookie) && strstr(_ip, ip)
    &&  strstr(_session, session)
      )
      return false;
      return true;
}

bool
userdb_setSession(  const char* const id,
                    const char* const session,
                    const char* const ip)
{
}

bool
userdb_checkLogin(  const char* const uname,
                    const char* const password){}

bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie )
{
}
