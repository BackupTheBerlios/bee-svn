#if !defined SUT_H
#define SUT_H
bool
sut_start( const int test_type, const int timeout, const char* maillog, const char* start, const char* host, const int port);

bool sut_stop( const int test_type, const int timeout, const char* maillog, const char* stop,const char* host,const int port);

bool sut_refresh( const int test_type,
                 const char* source, const char* dest, const char* host, const int port);

bool
sut_checkCore( const int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir );

void sut_sigpipe(int);
void sut_sigint(int);
int  sut_checkTools(const char* tools_path) ;
int  sut_runTests(const char* dir) ;

char*
expand_vars( char const * t1 );

char** sut_listTests(const char* td, int* sz) ;
char** sut_listMachines(const char* td, int* sz) ;

#else
        #warning "*** Header allready included ***"
#endif
