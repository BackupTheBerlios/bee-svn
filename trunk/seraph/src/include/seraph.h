#ifndef TESTBOT_H
#define TESTBOT_H

void srph_usage( int status ) ;
int  srph_parseArgs( struct config_s* cfg, int argc, char* args[]);
int  srph_initCfg( struct config_s* cfg, int argc, char* argv[]);
int  scan_parseCfg(char* config_file, void* arg) ;
int  srph_initEnv(struct config_s*) ;
int  srph_free(struct config_s*) ;
#else
        #warning "*** Header allready included ***"
#endif
