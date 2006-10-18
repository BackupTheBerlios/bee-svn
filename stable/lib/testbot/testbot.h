#ifndef TESTBOT_H
#define TESTBOT_H

#include <sys/stat.h>
#include "config.h"


static int  tb_ptRefresh(const char *filename);
static int  tb_checkCore(const char* core_srcDir, const char* dbg_srcDir, const char* axi_workDir,
                         const char* axi_cfgFile, const char* crash_destDir ) ;
static int  tb_checkTools(const char* tools_path) ;
static int  tb_cleanupTmp(void) ;
static int  tb_setupTmp( const char *source_bat) ;
static int  tb_envInit(struct config_s) ;
static int  tb_parseArgs( struct config_s cfg, int argc, char* args[]);
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
