#ifndef TESTBOT_H
#define TESTBOT_H

#include <sys/stat.h>
#include "config.h"


void tb_usage( int status ) ;
int  tb_parseArgs( struct config_s* cfg, int argc, char* args[]);
int  tb_cfgInit( struct config_s* cfg, int argc, char* argv[]);
int  tb_cfgParse(char* config_file) ;
int  tb_envInit(struct config_s*) ;
int  tb_checkTools(const char* tools_path) ;
int  tb_runTests(const char* dir) ;

int
expand_env(char * str, char* varName, int maxLen );
#endif
