#if !defined CORE_H
#define CORE_H

bool core_checkCore(  const int test_type,
                const char *core_srcDir, const char *dbg_srcDir,
                const char *axi_workDir, const char *axi_cfgFile,
                const char *crash_destDir );

int   core_checkTools(const char* tools_path) ;
char* core_expandVars( char const * t1 );
int   core_setErrorlog( void );
int   core_runTests(const char* dir) ;

#else
        #warning "*** Header allready included ***"
#endif
