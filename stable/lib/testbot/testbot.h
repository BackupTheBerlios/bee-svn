#ifndef TESTBOT_H
#define TESTBOT_H

#include <sys/stat.h>
#include "config.h"


void tb_usage(void) ;
int  tb_parseArgs( struct config_s* cfg, int argc, char* args[]);
int  tb_cfgInit( struct config_s* cfg, int argc, char* argv[]);
int  tb_cfgParse(char* config_file) ;
int  tb_envInit(struct config_s*) ;
int  tb_checkTools(const char* tools_path) ;
int  tb_runTests(const char* dir) ;
static int  tb_ptRefresh(int option, const char *filename);
static int  tb_checkCore(int test_type, const char* core_srcDir, const char* dbg_srcDir, const char* axi_workDir,
                         const char* axi_cfgFile, const char* crash_destDir ) ;
static int  tb_cleanupTmp(char* tmpDir) ;
static int  tb_setupTmp( const char *source_bat, char* tmpDir) ;
static int  tb_parseBat(const char *filename) ;
static int  tb_runBat(const char *bat_name, int timeout);
static int         tb_runRecursive(const char *srcName) ;
static int  tb_dirAction(const char *fileName, struct stat *statbuf,  void *junk) ;
static int  tb_fileAction(const char *fileName, struct stat *statbuf, void *junk);
static int  tb_setErrorlog(void) ;
int
expand_env(char * str, char* varName, int maxLen );
#endif
