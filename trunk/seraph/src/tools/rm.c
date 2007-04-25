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

/**
 *   \brief    Wraps around rm command.
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"
#include "fileop.h"

struct      config_s cfg;
static int  rm_parseArgs( int argc, char *argv[] );

int
main( int argc, char *argv[] )
{
    char    *tc=NULL;
    char    *host=NULL;
    int port=0, test_type=TEST_LOCAL;

    if( argc < 2 )
    {       printf( "! rm: missing file operand\n" );
        printf( "Try `rm -h` for more information.\n" );
        return 1;
    }
    rm_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    tc = getenv( SUT_TTYPE );
    if( argc < 2 ) {
        fprintf( stderr, "E: mkdir: Invalid syntax" );
        return 1;
    }

    if( !strcmp( tc, "local" ) )
        test_type = TEST_LOCAL;
    else if( !strcmp( tc, "remote" ) )
        test_type = TEST_REMOTE;
    else
        printf( "E: mkdir: Invalid test type $SUT_TTYPE\n" );
    fop_rm( test_type, argv[optind], host, port );
    return 1;
}



void rm_usage( int status )
{

    printf( "Usage: rm [OPTION] FILE...\n" );
    printf( "Remove (unlink) the FILE(s).\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( status );
}



static int rm_parseArgs( int argc, char *argv[] )
{
    int c;
    while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
        switch ( c ) {
            case 't':
                if( !strcasecmp( optarg, "remote" )
                        || ( !strcasecmp( optarg, "local" ) ) )
                    setenv( SUT_TTYPE, optarg, 1 );
                break;
            case 'H':
                setenv( SUT_HOST, optarg, 1 );
                break;
            case 'P':
                setenv( SUT_PORT, optarg, 1 );
                break;
            case 'h':
                rm_usage( EXIT_SUCCESS );
            case 'v':
                cfg.verbose = TRUE;
                break;
        }
    }
    return TRUE;
}
