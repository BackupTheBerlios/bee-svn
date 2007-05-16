#ifndef _USERDB_H_
#define _USERDB_H_
enum JobType {
    JOB_PENDING=1,
    JOB_RUNNING=2,
    JOB_COMPLETE=4,
    JOB_ALL=8
};

struct job {
    char *name ;
    char *date;
    char *time ;
    char *tests;
    char *ctest;
};



bool
userdb_register( const char* const name, const char* const email,
                 const char* const uName, const char* const pass);
bool
userdb_login( const char* uName, const char* pass);
int
userdb_addJob( int job_type );
int
userdb_listJobs( const char * const uName, enum JobType job_type, GSList** jobs);
bool
userdb_checkSession(const char* const uName,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip );
bool
userdb_setSession(  const char* const uName,
                    const char* const session,
                    const char* const ip);
bool
userdb_checkLogin(  const char* const uName,
                    const char* const password);
bool
userdb_checkRemembered( const char* const uName,
                        const char* const cookie );
size_t
userdb_getErrorLog( const char * uName, int job_type, const char*const log, char* *ret);
#endif
