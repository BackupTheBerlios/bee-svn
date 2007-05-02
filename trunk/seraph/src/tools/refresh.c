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
 *   \brief    Handles the refreshing of ptgen state(users/domains).
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"



struct config_s cfg;
static bool rc_parseArgs( int argc, char *argv[] );



int
main( int argc, char *argv[] )
{
    char    *tc;
    int     test_type = TEST_LOCAL;
    int     ret;

    DBG("refresh.debug");
    rc_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    str_isEnv( SUT_STOP );
    str_isEnv( SUT_START );
    str_isEnv( SUT_DEFWD );
    str_isEnv( SUT_WORKDIR );
    tc = getenv( SUT_TTYPE );

    if( !strcasecmp(tc, "local") )
        test_type = TEST_LOCAL;
    else if( !strcasecmp(tc, "remote") )
    {   str_isEnv( SUT_HOST );
        str_isEnv( SUT_PORT );
        test_type = TEST_REMOTE;
        cfg.hostname = getenv( SUT_HOST );
        cfg.rawport = atoi( getenv(SUT_PORT) );
    } else
    {   printf( "E: refresh : Invalid test type $SUT_TTYPE\n" );
        UNDBG;
        exit(EXIT_FAILURE);
    }

    system( getenv(SUT_STOP) );
    sut_refresh( test_type, getenv( SUT_DEFWD ), getenv( SUT_WORKDIR ), cfg.hostname, cfg.rawport );
    ret = system( getenv(SUT_START) );
    UNDBG;
    exit( ret);
}

void rc_usage( int status )
{
    printf( "Usage: refresh [OPTION] COMMAND...\n" );
    printf( "Refresh the state of serverUnderTest.\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( status );
}



static bool rc_parseArgs( int argc, char *argv[] )
{
    int c;
    while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 )
    {   switch ( c ) {
            case 't':
                if( !strcasecmp(optarg, "remote") ) {
                    setenv( SUT_TTYPE, optarg, 1 );
                    cfg.test_type = TEST_REMOTE;
                }
                if( !strcasecmp( optarg, "local" ) ) {
                    setenv( SUT_TTYPE, optarg, 1 );
                    cfg.test_type = TEST_LOCAL;
                }
                break;
            case 'H':
                setenv( SUT_HOST, optarg, 1 );
                cfg.hostname = optarg;
                break;
            case 'P':
                setenv( SUT_PORT, optarg, 1 );
                cfg.rawport = atoi( optarg );
                break;
            case 'h':
                UNDBG;
                rc_usage( EXIT_SUCCESS );
            case 'v':
                cfg.verbose = true;
                break;
        }
    }
    return true;
}
