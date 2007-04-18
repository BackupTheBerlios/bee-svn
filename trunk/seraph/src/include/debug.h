/** @file  Debug.h
 *  @brief Declarations of debug functions and Macros. */

#if !defined DEBUG_H
#define DEBUG_H

#include <stdlib.h>     /* getenv */
#include <stdio.h>
#include <errno.h>
#include <assert.h>


/*! Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV */
extern char* useDebug_g ;
extern FILE* logHandle ;

#ifndef debug
    #define debug(a,...) _debug( __FUNCTION__, __LINE__, __FILE__,a, ##__VA_ARGS__)
#endif

/*! Use this macro to activate the calling of debug()*/
#if defined USE_DEBUG
#if !defined DBG
    #define DBG(afile) useDebug_g =getenv("DBG") ;\
    logHandle = fopen( afile, "a" ) ;\
    if( !logHandle ){ printf( "Can't open '%s' [%s]\n", afile, strerror(errno)) ;exit(0) ;} \
    setlinebuf(logHandle);
#endif

#if !defined UNDBG
    #define UNDBG fclose( logHandle ) ;
#endif
#else
    #define DBG(a)
    #define UNDBG
#endif

void
_debug( const char* function, int line, const char* fileName, const char* format, ...) ;

#endif    /*DEBUG_H*/
