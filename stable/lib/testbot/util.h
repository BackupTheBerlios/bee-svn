#ifndef UTIL_H
#define UTIL_H

#include <wait.h>
#include "global.h"

int   client_rm(char* host, int port, char* path);
int   util_axiStart(int test_type, int timeout, char *start) ;
int   util_axiStop(int test_type, int timeout, char *start);
int   util_checkCoreLocal(const char* core_srcDir, const char* dbg_srcDir, const char* axi_workDir,
                         const char* axi_cfgFile, const char* crash_destDir ) ;

int   util_fileSize( char* name ) ;
int   util_getCode(char* str) ;
int   util_isEnv(char* var_name);
int   util_matches( char* buffer, char* string );
int   util_sendLine( int sock, char* line ) ;
int   util_sendStatus(int cod, int socket);
int   util_startsWith(char* str, char* exp) ;
char* util_getLine( int sock) ;
void  util_terminare() ;
int   util_endsWith(const char *str, const char *model) ;
int   recursiveAction(const char *fileName, int recurse, int followLinks, int depthFirst,
          int (*fileAction) (const char *fileName, struct stat* statbuf, void* userData),
          int (*dirAction) (const char *fileName, struct stat* statbuf, void* userData),
          void* userData);
int util_strsrch( const char* hay_stack, int hay_size, const char* needle, int needle_size ) ;
int   my_rm( char*);
#endif
