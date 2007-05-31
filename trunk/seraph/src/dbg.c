/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/** @file  Debug.cpp
 *  @brief Usefull in debugging. */
#include "dbg.h"
#include <stdio.h>  /*printf, fprintf*/
#include <stdarg.h> /*va_list*/


int useDebug_g=0 ;    //!< Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV
FILE* logHandle =0 ;

/** Print debug information in this.log.
 * @param[in] function Name of the calling function.
 * @param[in] line Line from where debug() was called.
 * @param[in] fname File name from where debug() was called.
 * @param[in] fmt Simmilar to printf. */
void
_debug( const char* function, int line, const char* fname, const char* fmt, ... )
{
    if( 0 == useDebug_g ) return ;

    va_list vlist ;
    va_start( vlist, fmt ) ;
        if( 2== useDebug_g) fprintf( stdout, "DBG: %15s %5d : [%10s] : ", fname, line, function ) ;
        fprintf( logHandle, "DBG: %15s %5d : [%10s] : ", fname, line, function ) ;
        fflush( logHandle ) ;
        vfprintf( logHandle, fmt, vlist ) ;
        if( 2== useDebug_g) vfprintf( stdout, fmt, vlist ) ;
    va_end( vlist ) ;
    fflush( stdout);
    fflush( logHandle ) ;
}
