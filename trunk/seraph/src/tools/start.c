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
 *   \brief    
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"

//! @todo use sut_binpath to look for serverUnderTest
struct config_s cfg;
char *start_cmd = NULL;
static int rc_parseArgs( int argc, char *argv[] );

static void start_usage( int status );

int main( int argc, char *argv[] )
{
        char *tc;
        int test_type = TEST_LOCAL;

        rc_parseArgs( argc, argv );
        str_isEnv( SUT_TTYPE );
        tc = getenv( SUT_TTYPE );

        if( !start_cmd ) {
                fprintf( stderr, "E: start: -c flag is mandatory\n" );
                start_usage( EXIT_FAILURE );
        }

        if( !strcasecmp( tc, "local" ) ) {
                printf( "* start: Working Local\n" );
                test_type = TEST_LOCAL;
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* start: Working Remote\n" );
                str_isEnv( SUT_HOST );
                str_isEnv( SUT_PORT );

                test_type = TEST_REMOTE;
                cfg.hostname = getenv( SUT_HOST );
                cfg.port = atoi( getenv( SUT_PORT ) );
        } else {
                printf( "E: start : Invalid test type $SUT_TTYPE\n" );
                return 1;
        }
        sut_start( test_type, 5, getenv( SUT_SYSLOG ), start_cmd,
                   cfg.hostname, cfg.port );
        return EXIT_SUCCESS;
}

static void start_usage( int status )
{

        printf( "Usage: start [OPTION] COMMAND...\n" );
        printf( "Start.\n" );
        printf( "\n" );
        printf
            ( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n" );
        printf( "  -P port\n" );
        printf( "  -c params\n" );
        printf( "  -t testType\n" );

        exit( status );
}



static int rc_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:c:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" ) ) {
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
                        cfg.port = atoi( optarg );
                        break;
                case 'c':
                        start_cmd = optarg;
                        break;
                case 'h':
                        start_usage( EXIT_SUCCESS );
                case 'v':
                        cfg.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
