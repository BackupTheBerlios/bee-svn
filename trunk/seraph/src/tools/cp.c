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
#include "config.h"
#include "dbg.h"
#include "fileop.h"
#include "sock.h"
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
    {   dbg_error( "cp: Cannot open source file [%s] : [%s]\n",
                src_file, strerror(errno) );
        return false;
    }

    while( (r = read( f, buff, BUFF_SZ)) >0 )
    {   t = 0;
        while( t < r )
        {   if( ( w = write( sock, buff + t, r - t ) ) < 0 )
            {   dbg_error( "cp: Can't send file [%s]: [%s]\n",
                        src_file, strerror(errno) );
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
    char    cmd[PATH_MAX] = { 0 };
    char    *bname=NULL, sockfd;

    sockfd = sock_connectTo( host, port );
    if(sockfd==-1)
        return false;

    l = fop_fileSize( src_file );
    if( l == -1 )
    {   dbg_error("cp: [%s] source file is not a regular file\n",
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
    {   dbg_error( "cp: Didn't Received command[cp] confirmation!\n" );
        shutdown( sockfd, 2);
        close( sockfd );
        return false ;
    }
    if(ret)
    {   dbg_error( "cp: Error while copying [%s] -> [%s]: [%s]!\n", bname, dest_dir, strerror(ret) );
        shutdown( sockfd, 2);
        close( sockfd );
        return false ;
    }
    dbg_verbose("cp: [%s] -> [%s] copied ok\n", src_file, dest_dir);
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
    {   dbg_error( "cp: missing file operand\n" );
        dbg_error( "Try `cp -h` for more information.\n" );
        exit(EXIT_FAILURE);
    }
    cfg.verbose = getenv(SUT_VERBOSE);

    DBG("cp.debug");
    cp_parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    type = getenv( SUT_TTYPE );

    if( !strcmp(type, "local") )
    {   char *cp_cmd=NULL;
        cp_cmd = (char*)malloc( strlen(argv[optind]) + strlen(argv[optind+1]) + 13 );
        /* TODO: use fop_cp */
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
        dbg_error( "cp: Invalid test type $SUT_TTYPE\n" );
    UNDBG;
    exit(EXIT_FAILURE);
}

void
cp_usage( int status )
{
    printf( "Usage: cp [OPTION] COMMAND...\n" );
    printf( "Copy SOURCE to DEST\n" );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H hostname\n" );
    printf( "  -P port\n" );
    printf( "  -t testType\n" );
    exit( status );
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
                cp_usage( EXIT_SUCCESS );
            case 'v':
                cfg.verbose = true;
                break;
            case '?':
                if (isprint (optopt))
                    {dbg_error("Unknown option `-%c'.\n", optopt);}
                else
                    dbg_error("Unknown option character `\\x%x'.\n", optopt);
                cp_usage( EXIT_FAILURE);
                break;
           default:
                cp_usage( EXIT_FAILURE);
                break;

        }
    }
    return true;
}
