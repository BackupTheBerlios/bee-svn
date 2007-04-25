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
 *   \brief     Wraps around copy operations.
 */
#include <strings.h>
#include <libgen.h>      /*basename*/
#include "config.h"
#include "fileop.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"

struct     config_s cfg;
static int cp_parseArgs( int argc, char *argv[] );


/*
 * Send a file to socket.
 */
static bool
sndfile( int sock, char *src_file )
{
    #define BUFF_SZ 8192

    char    buff[BUFF_SZ]={0};
    int     t=0, r=0, w=0;

    int f = open( src_file, O_RDONLY );
    if( f < 0 )
    {   fprintf( stderr, "E: cp: Cannot open source file [%s] : %s\n",
                src_file, strerror( errno ) );
        return false;
    }

    while( (r = read( f, buff, BUFF_SZ)) >0 )
    {   t = 0;
        while( t < r )
        {   if( ( w = write( sock, buff + t, r - t ) ) < 0 )
            {   fprintf( stderr,
                        "E: cp: Can't send file [%s]: %s\n",
                        src_file, strerror( errno ) );
            }
            t += w;
            debug("left to write:%d\n", r-t);
        }
    }
    close( f );
    return true;
}


static bool
copy_remote( char *host, int port, char *src_file, char *dest_dir )
{
    int     ret=0,l=0;
    char    cmd[LINE_MAX] = { 0 };
    char    *bname=NULL, sockfd;

    sockfd = sock_connectTo( host, port );

    l = fop_fileSize( src_file );
    if( l == -1 )
    {   fprintf( stderr,
                "E: cp: [%s] source file is not a regular file\n",
                src_file );
        shutdown( sockfd, 2);
        close(sockfd);
        return false ;
    }

    bname = basename( src_file );
    sprintf( cmd, "COPY %s %s %d", bname, dest_dir, l );
    sock_sendLine( sockfd, cmd );
    ret = sndfile( sockfd, src_file );
    if(!ret)
    {   shutdown( sockfd, 2);
        close(sockfd);
        return false;
    }

    ret = sock_getStatus( sockfd );
    if( ret==-1 )
    {   fprintf( stderr,"E: cp: Didn't Received command[cp] confirmation!\n" );
        shutdown( sockfd, 2);
        close( sockfd );
        return false ;
    }
    if(ret)
    {   fprintf( stderr,"E: cp: Error while copying [%s]!\n", strerror(ret) );
        shutdown( sockfd, 2);
        close( sockfd );
        return false ;
    }
    dbg_verbose("cp: file [%s] copied ok\n", src_file);
    shutdown( sockfd, 2);
    close(sockfd);
    return true;
}



int
main( int argc, char *argv[] )
{
    char    *type="local";
    int     ret=0;

    if( argc < 2 )
    {   printf( "E: cp: missing file operand\n" );
        printf( "Try `cp -h` for more information.\n" );
        exit(EXIT_FAILURE);
    }

    DBG("cp.debug");
    cp_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    type = getenv( SUT_TTYPE );

    if( !strcmp(type, "local") )
    {   char *cp_cmd=NULL;
        cp_cmd = (char*)malloc( strlen(argv[optind]) + strlen(argv[optind+1]) + 13 );
        sprintf( cp_cmd, "/bin/cp -R %s %s", argv[optind], argv[optind+1] );
        ret = system( cp_cmd );
        free(cp_cmd);
        UNDBG;
        exit( ret );
    } else if( !strcmp(type, "remote") )
    {   char *host=NULL;
        int port=0;

        str_isEnv( SUT_HOST );
        str_isEnv( SUT_PORT );
        host = getenv( SUT_HOST );
        port = atoi( getenv( SUT_PORT ) );
        ret = copy_remote( host, port, argv[optind], argv[optind + 1] );
        UNDBG;
        ret ? exit(EXIT_SUCCESS):exit(EXIT_FAILURE);
    } else
        printf( "E: cp: Invalid test type $SUT_TTYPE\n" );
    UNDBG;
    exit(EXIT_FAILURE);
}

void
cp_usage( void )
{
    printf( "Usage: cp [OPTION] COMMAND...\n" );
    printf( "Copy SOURCE to DEST\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( EXIT_SUCCESS );
}


static int
cp_parseArgs( int argc, char *argv[] )
{
    int     c;
    while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 )
    {   switch ( c ) {
            case 't':
                if( !strcasecmp( optarg, "remote" ) || ( !strcasecmp( optarg, "local" ) ) )
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
                cp_usage( );
            case 'v':
                cfg.verbose = true;
                break;
        }
    }
    return true;
}
