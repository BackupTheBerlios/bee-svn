
bool
sut_start( int test_type, int timeout, char* maillog, char* start, char* host, int port);
static bool sut_startRemote( int timeout, char* maillog, char* start, char* host, int port);
static bool sut_startLocal( int timeout, char* maillog, char* start);

bool sut_stop( int test_type, int timeout, char* maillog, char* stop, char* host, int port);
static bool sut_stopRemote( int timeout, char* maillog, char* stop, char* host, int port);
static bool sut_stopLocal( int timeout, char* maillog, char* stop);

bool sut_refresh( int test_type,
                 char* source, char* dest, char* host, int port);
static bool sut_refreshRemote( char* host, int port, char* source, char* dest);
static bool sut_refreshLocal( char* source, char* dest);

bool
sut_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir );
static bool
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                    const char *axi_workDir, const char *axi_cfgFile,
                    const char *crash_destDir );
static bool
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                     const char *workDir, const char *cfgFile,
                     const char *crash_destDir );

void sut_sigpipe();
void sut_sigint();
