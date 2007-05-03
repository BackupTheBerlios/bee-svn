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
#include "config.h"
#include "dbg.h"
#include "sock.h"
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
    int     port=0, test_type=TEST_LOCAL, ret=0;

    if( argc < 2 )
    {   printf( "E: rm: missing file operand\n" );
        printf( "Try `rm -h` for more information.\n" );
        exit(EXIT_FAILURE);
    }

    DBG("rm.debug");
    rm_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    tc = getenv( SUT_TTYPE );
    if( argc < 2 )
    {   fprintf( stderr, "E: rm: Invalid syntax" );
        UNDBG;
        exit(EXIT_FAILURE);
    }

    if( !strcmp(tc, "local") )
        test_type = TEST_LOCAL;
    else if( !strcmp(tc, "remote") )
    {   str_isEnv( SUT_HOST);
        str_isEnv( SUT_PORT);

        host = getenv( SUT_HOST);
        port = atoi( getenv(SUT_PORT));
        test_type = TEST_REMOTE;
    }
    else
        printf( "E: rm: Invalid test type $SUT_TTYPE\n" );

    ret = fop_rm( test_type, argv[optind], host, port );
    UNDBG;
    exit( ret);
}



void
rm_usage( int status )
{
    printf( "Usage: rm [OPTION] FILE...\n" );
    printf( "Remove (unlink) the FILE(s).\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H host\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( status );
}



static int
rm_parseArgs( int argc, char *argv[] )
{
    int     c;
    while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 )
    {   switch ( c ) {
            case 't':
                if( !strcasecmp( optarg, "remote" )
                || (!strcasecmp( optarg, "local" ))
                  )
                    setenv( SUT_TTYPE, optarg, 1 );
                break;
            case 'H':
                setenv( SUT_HOST, optarg, 1 );
                break;
            case 'P':
                setenv( SUT_PORT, optarg, 1 );
                break;
            case 'h':
                UNDBG;
                rm_usage( EXIT_SUCCESS );
                break;
            case 'v':
                cfg.verbose = true;
                break;
        }
    }
    return true;
}
