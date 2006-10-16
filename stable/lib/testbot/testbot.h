#ifndef TESTBOT_H
#define TESTBOT_H

static int  tb_putRefresh(const char *filename);
static int  tb_checkCore(const char* core_srcDir, const char* dbg_srcDir, const char* put_workDir,
                         const char* put_cfgFile, const char* crash_destDir ) ;
static int  tb_checkCoreRemote(const char* core_srcDir, const char* dbg_srcDir, const char* put_workDir,
                               const char* put_cfgFile, const char* crash_destDir ) ;
static int  tb_checkTools(const char* tools_path) ;
static int  tb_cleanupTmp(void) ;
static int  tb_setupTmp( const char *source_bat) ;
static int  tb_envInit(void) ;
static int  tb_globalsInit(int argc, char* argv[]) ;
static int  tb_logPrint(const int cod, const char *name) ;
static int  tb_parseArgs( int argc, char* args[]);
static int  tb_parseBat(const char *filename) ;
static int  tb_parseConf(void) ;
static int  tb_runBat(const char *bat_name);
static int  tb_runTests(const char* dir) ;
int         tb_runRecursive(const char *srcName) ;
static int  tb_dirAction(const char *fileName, struct stat *statbuf,  void *junk) ;
static int  tb_fileAction(const char *fileName, struct stat *statbuf, void *junk);
static int  tb_setErrorlog(void) ;
static void tb_usage(void) ;
int
expand_env(char * str, char* varName, int maxLen );
    
#endif
