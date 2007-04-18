bool
userdb_register( const char* uname, const char* pass)
{
    chdir(USERDB);
    /**
     * TODO add jobs dir aswell */
    if( mkdir(uname) == -1)
        return false;
    chdir(uname);
    if(!fopen(pass, "w"))
        return false;
    fprintf(f, "%s", pass);
    fclose(f);
    return true;
}
bool
userdb_login( const char* uname, const char* pass)
{
    char* p;
    if( !access(USERDB/uname) )
        return false;
    if(!fopen(pass, "r") )
        return false;
    fscanf(f, "%s", p );
    if( strstr(p, pass) )
        return false;
    return true;
}

int
userdb_addJob( int job_type )
{
    return 0;
}


int
userdb_listJobs( int job_type, GSList** jobs)
{
    int nbJobs=0;
    DIR* dit;
    struct dirent* dent;

    if(job_type & JOBS_ALL )
    {
        job_type |= JOB_PENDING;
        job_type |= JOB_RUNNING;
        job_type |= JOB_COMPLETE;
    }
    if(job_type & JOBS_PENDING )
    {
        dit = opendir(USERDB/uname/jobs/pending);
        while( dent=readdir(dit) )
        {
            if( !strstr(dent->d_name,".") || !strstr(dent->d_name, "..") ) continue;
            ++nbJobs;
            gslist_append(*jobs, dent->d_name);
        }
        closedir(dit);
    }
    if(job_type & JOBS_RUNNING )
    {
        dit  = opendir(USERDB/uname/jobs/running);
        while( dent=readdir(dit) )
        {
            if( dir == "." || dir == "..") continue;
            ++nbJobs;
            gslist_append(*jobs, str);
        }
        close( dit );
    }
    if(job_type & JOBS_COMPLETE )
    {
        dit = opendir(USERDB/uname/jobs/complete);
        while( dent=readdir(dit) )
        {
            if( dir == "." || dir == "..") continue;
            ++nbJobs;
            gslist_append(*jobs, str);
        }
        close( dit );
    }
    return nbJobs;
}
