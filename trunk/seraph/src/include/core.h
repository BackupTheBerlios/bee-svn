#if !defined CORE_H
#define CORE_H

bool core_checkCore(  const int test_type,
                const char *coreSrcDir, const char *dbgSrcDir,
                const char *sutWorkDir, const char *sutCfgFile,
                const char *dumpDestDir );

int   core_checkTools(const char* tools_path) ;
char* core_expandVars( char const * t1 );
int   core_setErrorlog( const char* const username, const char* const test_dir );
int   core_runTests(const char* dir) ;
void core_onSigChld( int sig );
void core_onSigInt(  int sig);
void core_onSigPipe(  int sig);
void core_execcmd( char *string );
int  core_execwait();

#else
        #warning "*** Header allready included ***"
#endif
