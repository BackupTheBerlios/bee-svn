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
#include "debug.h"
#include <stdio.h>  //printf, fprintf
#include <stdarg.h> //va_list


char* useDebug_g=(char*)1 ;    //!< Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV
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
        fprintf( logHandle, "%15s %5d : [%10s] : ", fname, line, function ) ;
        fprintf( stdout, "%15s %5d : [%10s] : ", fname, line, function ) ;
        fflush( logHandle ) ;
        vfprintf( logHandle, fmt, vlist ) ;
        vfprintf( stdout, fmt, vlist ) ;
    va_end( vlist ) ;
    fflush( logHandle ) ;
}
