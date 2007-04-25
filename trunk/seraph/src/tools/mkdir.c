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
 *   \brief    Wraps around mkdir cmd.
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"



struct      config_s cfg;
static int  mkdir_parseArgs( int argc, char *argv[] );



static bool
mkdir_remote( char *host, int port, char *path )
{
    char    *cmd=NULL;
    int     ret = 0, sockfd = -1;

    cmd = (char*)malloc(strlen(path)+8);
    sockfd = sock_connectTo( host, port);
    sprintf( cmd, "MKDIR %s", path);
    sock_sendLine( sockfd, cmd);
    ret = sock_getStatus( sockfd);
    if( ret )
    {   fprintf( stderr,
                "E: mkdir: cannot create directory [%s]: %s\n", path, strerror(ret) );
        shutdown( sockfd, 2);
        close( sockfd);
        free( cmd);
        return false;
    }
    dbg_verbose("mkdir: Directory [%s] created ok\n", path);
    free( cmd);
    shutdown( sockfd, 2);
    close( sockfd );
    return true;
}



int
main( int argc, char *argv[] )
{
    char    *tc=NULL, *host=NULL;
    int     port=0;
    bool    ret=false;

    if( argc < 2 )
    {   printf( "E: mkdir: missing operand\n" );
        printf( "Try `mkdir -h` for more information.\n" );
        exit(EXIT_FAILURE);
    }

    DBG("mkdir.debug");
    mkdir_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    tc = getenv( SUT_TTYPE );

    if( !strcmp(tc, "local") )
    {   if(mkdir( argv[optind], 0755))
        {   printf("E: mkdir: cannot create directory [%s]: %s\n",
                    argv[optind], strerror(errno) );
            UNDBG;
            exit(EXIT_FAILURE);
        }
        dbg_verbose( "mkdir: [%s] created\n", argv[optind]);
        UNDBG;
        exit( EXIT_SUCCESS);
    } else if( !strcmp(tc, "remote") )
    {   str_isEnv( SUT_HOST);
        str_isEnv( SUT_PORT);
        host = getenv( SUT_HOST);
        port = atoi( getenv(SUT_PORT) );
        if(argv[optind])
            ret = mkdir_remote( host, port, argv[optind]);
        UNDBG;
        ret? exit(EXIT_SUCCESS): exit(EXIT_FAILURE);
    } else
        printf( "E: mkdir: Invalid test type $SUT_TTYPE\n" );
    UNDBG;
    exit(EXIT_FAILURE);
}



void
mkdir_usage( void )
{
    printf( "Usage: mkdir [OPTION] COMMAND...\n" );
    printf( "Create the DIRECTORY, if they do not already exist.\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( 0 );
}



static int
mkdir_parseArgs( int argc, char *argv[] )
{
    int c;
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
                mkdir_usage( );
                break;
            case 'v':
                cfg.verbose = true;
                break;
        }
    }
    return true;
}
