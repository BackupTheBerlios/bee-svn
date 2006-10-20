/** @file  Debug.h
 *  @brief Declarations of debug functions and Macros. */

#if !defined DEBUG_H
#define DEBUG_H

#include <stdlib.h>     // getenv
#include <exception>
#include <stdexcept>
#include <errno.h>
#include <assert.h>


//! Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV
extern char* useDebug_g ;
extern FILE* logHandle ;

#ifndef debug
    #define debug(a,...) _debug( __FUNCTION__, __LINE__, __FILE__,a, ##__VA_ARGS__)
#endif

//! Use this macro to activate the calling of debug()
#if !defined DBG
    #define DBG useDebug_g =getenv("DBG") ;\
    logHandle = fopen( "this.log", "a" ) ;\
    if( !logHandle ){ printf( "Can't open log\n") ;exit(0) ;} \
    setlinebuf(logHandle);
#endif

#if !defined UNDBG
    #define UNDBG fclose( logHandle ) ;
#endif


void
_debug( const char* function, int line, const char* fileName, const char* format, ...) ;

void*
operator new( size_t sz) ;

void
operator delete( void*) ;

#endif    //DEBUG_H
