#ifndef TESTBOT_H
#define TESTBOT_H

#include "config.h"

void srph_usage( int status ) ;
int  srph_parseArgs( struct config_s* cfg, int argc, char* args[]);
int  srph_initCfg( struct config_s* cfg, int argc, char* argv[]);
int  srph_parseCfg(char* config_file) ;
int  srph_initEnv(struct config_s*) ;
int  srph_free(struct config_s*) ;
int  srph_checkTools(const char* tools_path) ;
int  srph_runTests(const char* dir) ;

char*
expand_vars( char const * t1 );
#else
        #warning "*** Header allready included ***"
#endif
