
bool
userdb_register( const char* const name, const char* const email,
                 const char* const uname, const char* const pass);
bool
userdb_login( const char* uname, const char* pass);
int
userdb_addJob( int job_type );
int
userdb_listJobs( const char * const uname, enum JobType job_type, GSList** jobs);
bool
userdb_checkSession(const char* const uname,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip );
bool
userdb_setSession(  const char* const uname,
                    const char* const id,
                    const char* const session,
                    const char* const ip);
bool
userdb_checkLogin(  const char* const uname,
                    const char* const password);
bool
userdb_checkRemembered( const char* const uname,
                        const char* const cookie );
