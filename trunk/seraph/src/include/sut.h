#if !defined SUT_H
#define SUT_H
bool
sut_start( int test_type, int timeout, char* maillog, char* start, char* host, int port);

bool sut_stop( int test_type, int timeout, char* maillog, char* stop, char* host, int port);

bool sut_refresh( int test_type,
                 char* source, char* dest, char* host, int port);

bool
sut_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir );

void sut_sigpipe();
void sut_sigint();
#else
        #warning "*** Header allready included ***"
#endif
