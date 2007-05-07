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

#include "config.h"
#include "dbg.h"
#include "strop.h"
#include "sock.h"
#include "fileop.h"

extern struct config_s cfg;

static int recursiveFlag = 0;
static int forceFlag = 0;

static int fop_rmRemote( const char *path, const char *host, const int port );
static int fop_rmLocal( const char *path );
static int rm_fileAction( const char *fileName, struct stat *statbuf, void *junk );
static int rm_dirAction( const char *fileName, struct stat *statbuf, void *junk );



int fop_fileSize( char *name )
{
        struct stat inf;

        if( stat( name, &inf ) < 0 ) {
                debug( "stat() error: :%s %s\n", name, strerror( errno ) );
                exit( -1 );
        }
        return inf.st_size;
}


int fop_rm( const int test_type, const char *path,const char *host,const int port )
{
        if( test_type == TEST_LOCAL )
                return fop_rmLocal( path );
        if( test_type == TEST_REMOTE )
                return fop_rmRemote( path, host, port );
        return false;
}


static int
fop_rmRemote( const char *path,const char *host, const int port )
{
    char    *cmd=NULL;
    int     ret = 0, sockfd = -1;

    dbg_verbose("rmRemote: path(%s) host(%s) port(%d)\n", path, host, port);
    cmd = (char*)malloc(strlen(path)+8);
    sockfd = sock_connectTo( host, port);
    if(sockfd==-1)
        return false;

    sprintf( cmd, "RMDIR %s", path);
    sock_sendLine( sockfd, cmd);
    ret = sock_getStatus( sockfd);
    if( ret )
    {   fprintf( stderr,
                "E: rm: cannot delete directory [%s]: %s\n", path, strerror(ret) );
        shutdown( sockfd, 2);
        close( sockfd);
        free( cmd);
        return false;
    }
    dbg_verbose("rm: Directory [%s] deleted ok\n", path);
    free( cmd);
    shutdown( sockfd, 2);
    close( sockfd );
    return true;
}

static int fop_rmLocal( const char *path )
{
        struct stat statbuf;
        recursiveFlag = 1;
        forceFlag = true;
        if( forceFlag == true && lstat( path, &statbuf ) != 0
            && errno == ENOENT ) {
                /* do not reports errors for non-existent files if -f, just skip them */
        } else {
                if( recursiveAction( path, recursiveFlag, false,
                                     true, rm_fileAction, rm_dirAction,
                                     NULL ) == false ) {
                        exit( false );
                }
        }
        return 1;
}






/*-------------*/


/*Copyright busybox*/
/* 
 * Walk down all the directories under the specified 
 * location, and do something (something specified
 * by the fileAction and dirAction function pointers).
 *
 * Unfortunatly, while nftw(3) could replace this and reduce 
 * code size a bit, nftw() wasn't supported before GNU libc 2.1, 
 * and so isn't sufficiently portable to take over since glibc2.1
 * is so stinking huge.
 */
int
recursiveAction( const char *fileName,
                 int recurse, int followLinks, int depthFirst,
                 int ( *fileAction ) ( const char *fileName,
                                       struct stat * statbuf,
                                       void *userData ),
                 int ( *dirAction ) ( const char *fileName,
                                      struct stat * statbuf,
                                      void *userData ), void *userData )
{
        int status;
        struct stat statbuf;
        struct dirent *next;

        if( followLinks == true )
                status = stat( fileName, &statbuf );
        else
                status = lstat( fileName, &statbuf );

        if( status < 0 ) {
#ifdef BB_DEBUG_PRINT_SCAFFOLD
                debug( "status=%d followLinks=%d true=%d\n",
                        status, followLinks, true );
#endif
                perror( fileName );
                return false;
        }

        if( ( followLinks == false ) && ( S_ISLNK( statbuf.st_mode ) ) ) {
                if( fileAction == NULL )
                        return true;
                else
                        return fileAction( fileName, &statbuf, userData );
        }

        if( recurse == false ) {
                if( S_ISDIR( statbuf.st_mode ) ) {
                        if( dirAction != NULL )
                                return ( dirAction
                                         ( fileName, &statbuf, userData ) );
                        else
                                return true;
                }
        }

        if( S_ISDIR( statbuf.st_mode ) ) {
                DIR *dir;

                dir = opendir( fileName );
                if( !dir ) {
                        perror( fileName );
                        return false;
                }
                if( dirAction != NULL && depthFirst == false ) {
                        status = dirAction( fileName, &statbuf, userData );
                        if( status == false ) {
                                perror( fileName );
                                return false;
                        }
                }
                while( ( next = readdir( dir ) ) != NULL ) {
                        char nextFile[BUFSIZ + 1];

                        if( ( strcmp( next->d_name, ".." ) == 0 )
                            || ( strcmp( next->d_name, "." ) == 0 ) ) {
                                continue;
                        }
                        if( strlen( fileName ) + strlen( next->d_name ) + 1 >
                            BUFSIZ ) {
                                debug( "Name too long %s", "ftw" );
                                return false;
                        }
                        memset( nextFile, 0, sizeof( nextFile ) );
                        sprintf( nextFile, "%s/%s", fileName, next->d_name );
                        status =
                            recursiveAction( nextFile, true, followLinks,
                                             depthFirst, fileAction,
                                             dirAction, userData );
                        if( status < 0 ) {
                                if( -1 == closedir( dir ) )
                                        perror( "Unable to close dir" );
                                return false;
                        }
                }
                status = closedir( dir );
                if( status < 0 ) {
                        perror( fileName );
                        return false;
                }
                if( dirAction != NULL && depthFirst == true ) {
                        status = dirAction( fileName, &statbuf, userData );
                        if( status == false ) {
                                perror( fileName );
                                return false;
                        }
                }
        } else {
                if( fileAction == NULL )
                        return true;
                else
                        return fileAction( fileName, &statbuf, userData );
        }
        return true;
}




static int
rm_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
        if( unlink( fileName ) < 0 ) {
                perror( fileName );
                return ( false );
        }
        return ( true );
}


static int
rm_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
        if( rmdir( fileName ) < 0 ) {
                perror( fileName );
                return ( false );
        }
        return ( true );
}



bool fop_cp( const char* const s, const char* const d)
{
#define BUF_SZ 8192
    int src=-1, dst=-1, br=0;
    char buf[BUF_SZ]={0};

    src = open( s, O_RDONLY);
    dst = open( d, O_CREAT | O_EXCL | O_WRONLY , S_IWUSR | S_IRUSR );
    if( src<0 || dst<0)
    {   debug("Unable to create [%s] [%s]\n", dst, strerror(errno));
        return false;
    }
    while( (br=read( src, buf, BUF_SZ)) >0 )
    {
        debug("read %d\n", br);
        write( dst, buf, br);
    }
    close( src);
    close( dst);
    return true;
}
