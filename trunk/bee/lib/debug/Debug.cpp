/** @file  Debug.cpp
 *  @brief Usefull in debugging. */
#include "Debug.h"
#include <stdio.h>  //printf, fprintf
#include <stdarg.h> //va_list


char* useDebug_g=0 ;    //!< Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV
FILE* logHandle =0 ;

/** Print debug information in this.log.
 * @param[in] function Name of the calling function.
 * @param[in] line Line from where debug() was called.
 * @param[in] fname File name from where debug() was called.
 * @param[in] fmt Simmilar to printf. **/
void
_debug( const char* function, int line, const char* fname, const char* fmt, ... )
{
    if( 0 == useDebug_g ) return ;

    va_list vlist ;
    va_start( vlist, fmt ) ;
        fprintf( logHandle, "[%10u]--%15s +%d : [%10s] <", pthread_self(), fname, line, function ) ;
        fflush( logHandle ) ;
        vfprintf( logHandle, fmt, vlist ) ;
    va_end( vlist ) ;
    fprintf( logHandle, ">\n" ) ;
    //fflush( logHandle ) ;
}//*debug
