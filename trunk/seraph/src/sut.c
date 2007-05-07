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
#include "sock.h"
#include "strop.h"
#include "strop.h"
#include "fileop.h"
#include "sut.h"
/*
 * Dont use any GETENV, or cfgal variables in this file
 */

extern struct config_s cfg;

static bool sut_startRemote( const int timeout, const char *maillog, const char *start,
			     const char *host, const int port );

static bool sut_startLocal( const int timeout, const char *maillog, const char *start );

static bool sut_stopRemote( const int timeout, const char *maillog, const char *stop,
			    const char *host, const int port );

static bool sut_stopLocal( const int timeout, const char *maillog, const char *stop );

static bool sut_refreshRemote( const char *host, const int port, const char* refreshCmd );

static bool sut_refreshLocal( const char *refreshCmd);

static int sut_sutRefresh( const int option, const char *filename );

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_start( const int test_type,
           const int timeout, const char *maillog,
           const char *start, const char *hostname,const  int port )
{
    /*printf( "START [%d][%s][%s]\n", timeout, maillog, startCmd );*/

    if( test_type == TEST_LOCAL )
    {       sut_startLocal( timeout, maillog, start );
        sleep( 1 );
        return true;
    }

    if( test_type == TEST_REMOTE )
    {       sut_startRemote( timeout, maillog, start, hostname, port );
        sleep( 1 );
        return true;
    }
    return true;
}/*------------------------------------------------------------------*/



/*
 * \todo Test for failure */
    static bool
sut_startRemote( const int timeout, const char *maillog,
        const char *start, const char *hostname, const int port )
{
    int sockfd = -1;
    char cmd[FILENAME_MAX] = { 0 };

    sockfd = sock_connectTo( hostname, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "START %d %s %s", timeout, maillog, start );
    sock_sendLine( sockfd, cmd );
    close( sockfd );
    return true;
}/*------------------------------------------------------------------*/



/**
 * \brief wait until is started
 * Will read from maillog file, in a manner simmilar to
 * tail -f, looks for SUCCESS: supervise ready, and INFO: ready
 */
static bool sut_startLocal( const int timeout, const char *maillog, const char *start )
{
    int fd = 0, rc = 0;
    ino_t st_ino;
    struct stat stat_buf;
    char buf[512]   = { 0 };
    char *supRdyStr = "SUCCESS: supervise ready";
    char *rdyStr    = "INFO: ready";
    bool supRdy     = false;

    printf( "==> %s <==\n", maillog );
    fd = open( maillog, O_RDONLY );
    if( fd < 0 )
    {   debug( "E: Unable to open [%s] for reading\n", maillog );
        return false;
    }

    if( -1 == fstat( fd, &stat_buf ) )
    {   debug("E: Unable to stat [%s]: [%s]\n", maillog, strerror( errno ) );
        return false;
    }
    st_ino = stat_buf.st_ino;
    lseek( fd, 0, SEEK_END );
    rc = system( start );
    if( rc == -1 )
    {   printf( "Failed\n" );
        exit( EXIT_FAILURE );
        /* ask for options */
    }
    for( ;; ) {
        int i = 0;
        memset( buf, '\0', 512 );
        if( -1 == fstat( fd, &stat_buf ) ) {
            debug( "E: Unable to stat [%s]: [%s]\n",
                    maillog, strerror( errno ) );
            return false;
        }
        if( stat_buf.st_ino != st_ino ) {
            printf
                ( "[%s] has been replaced;  following end of new file",
                  maillog );
            close( fd );
            fd = open( maillog, O_RDONLY );
            if( -1 == fstat( fd, &stat_buf ) ) {
                fprintf( stderr,
                        "! sut: Unable to stat [%s]: [%s]\n",
                        maillog, strerror( errno ) );
                return false;
            }
            st_ino = stat_buf.st_ino;
            lseek( fd, 0, SEEK_END );
        }
        i = read( fd, buf, sizeof( buf ) - 1 );
        if( i < 0 ) {
            fprintf( stderr,
                    "! seraph: Cant read syslog '%s'\n", maillog );
            break;
        }
        if( i == 0 ) {
            continue;
        } else {
            buf[i] = '\0';
            printf( "%s", buf );
            /* Looking for a SUCCESS: supervise ready
             * a possible bug might be if SUPERVISER text gets in the same
             * buffer with INFO: ready. Then we get a false status*/
            if( !supRdy ) {
                rc = str_search( buf, i, supRdyStr,
                        strlen( supRdyStr ) );
                supRdy = rc;
                if( rc ) {
                    printf( "\n==> supervise ready <==\n" );
                    continue;
                }
            } else {
                rc = str_search( buf, i, rdyStr,
                        strlen( rdyStr ) );
                if( rc ) {
                    printf( "\n==> server started <==\n" );
                    close( fd );
                    return true;
                }
            }
        }
    }
    close( fd );
    return true;
}/*------------------------------------------------------------------*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_stop( const int test_type,
        const int timeout, const char *maillog, const char *stop, const char *hostname, const int port )
{
//    debug( "STOP [%d][%s][%s]\n", timeout, maillog, stopCmd );
    if( test_type == TEST_LOCAL ) {
        sut_stopLocal( timeout, maillog, stop );
        sleep( 1 );
        return true;
    }
    if( test_type == TEST_REMOTE ) {
        sut_stopRemote( timeout, maillog, stop, hostname, port );
        sleep( 1 );
        return true;
    }
    return true;
}/*------------------------------------------------------------------*/



    static bool
sut_stopRemote( const int timeout,
        const char *maillog, const char *stop, const char *hostname, const int port )
{

    int sockfd = -1;
    char cmd[FILENAME_MAX] = { 0 };
    sockfd = sock_connectTo( hostname, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "STOP %d %s %s", timeout, maillog, stop );
    sock_sendLine( sockfd, cmd );
    close( sockfd );
    return true;
}/*------------------------------------------------------------------*/



static bool sut_stopLocal( const int timeout, const char *maillog, const char *stop )
{
    int fd = 0, rc = 0;
    char buf[512] = { 0 };
    char *supRdyStr = "INFO: supervise: caught signal";
    char *rdyStr = "INFO: supervise: finished";
    bool supRdy = false;

    printf( "==> %s <==\n", maillog );
    fd = open( maillog, O_RDONLY );
    lseek( fd, 0, SEEK_END );
    rc = system( stop );
    if( rc == -1 ) {
        printf( "Failed\n" );
        exit( EXIT_FAILURE );
        /* ask for options */
        return false;
    }
    if( WEXITSTATUS( rc ) == 1 ) {
        printf( "No process killed\n" );
        return true;
    }
    for( ;; ) {
        int i = 0;
        memset( buf, '\0', 512 );
        i = read( fd, buf, sizeof( buf ) - 1 );
        if( i < 0 ) {
            fprintf( stderr, "! Cant read syslog '%s'\n", maillog );
            break;
        }
        if( i == 0 ) {
            /* sleep(1); */
            continue;
        } else {
            buf[i] = '\0';
            printf( "%s", buf );
            /* Looking for a SUCCESS: supervise ready */
            /* a possible bug might be if SUPERVISER text gets in the same
             * buffer with INFO: ready. Then we get a false status*/
            if( !supRdy ) {
                rc = str_search( buf, i, supRdyStr,
                        strlen( supRdyStr ) );
                supRdy = rc;
                if( rc )
                    printf
                        ( "\n==> supervise caught signal <==\n" );
            } else {
                rc = str_search( buf, i, rdyStr,
                        strlen( rdyStr ) );
                if( rc ) {
                    printf
                        ( "\n==> supervise finished <==\n" );
                    close( fd );
                    return true;
                }
            }
        }
    }
    return true;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool
sut_refresh( const int test_type, int refresh, const char* refreshCmd, const char *host, const int port )
{
    if( refresh == OPT_NO )
        return 0;

    if( refresh == OPT_ASK )
    {   char r = 0;
        do
        {   printf( "Reinstall default configuration? [y/n]: " );
            fflush(stdout);
            r = getchar(  );
            while( getchar(  ) != '\n' );
        } while( r != 'y' && r != 'n' );
        if( r == 'n' )
        {   dbg_verbose( "Keeping current state\n" );
            return 0;
        }
    }

    /*cod = sut_parseBat( "runtest.bat" );
    if( cod == OPT_NO )
        return 0;
    */
    /* We excluded all NO's, so we're left with the yes. */
    if( test_type == TEST_LOCAL ) {
        return sut_refreshLocal( refreshCmd );
    }

    if( test_type == TEST_REMOTE ) {
        if( !sut_refreshRemote( host, port, refreshCmd ) ) {
            dbg_error( "refresh: Could not make refresh.\n" );
            return false;
        }
        return true;
    }
    return false;
}/*------------------------------------------------------------------*/



/*
 * Restore the default configuration of the SUT.
 * SUT = server under test */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_refreshLocal( const char* refreshCmd )
{
    int cod;
    int rc = 0;

    dbg_verbose("Refreshing...\n");
    rc = system( refreshCmd );
    if( rc == -1 || WEXITSTATUS(rc) != 0 )
    {   dbg_error( "Can't make refresh!\n" );
        exit( EXIT_FAILURE );
    }

    return 0;
}


static bool sut_refreshRemote( const char *host, const int port, const char *refreshCmd )
{
    char command[PATH_MAX] = { 0 };
    int cod, sockfd;

    sockfd = sock_connectTo( host, port );
    if(sockfd==-1 )
    {   dbg_error("Unable to connect host[%s] port[%d]\n", host, port);
        return false;
    }

    sprintf( command, "REFRESH %s", refreshCmd);
    dbg_verbose("refresh [%s]\n", command);
    sock_sendLine( sockfd, command );
    cod = sock_getStatus( sockfd );
    if( cod < 0 )
    {   dbg_error("refresh: [%s]\n", strerror( cod ) );
        close( sockfd );
        return cod;
    }

    close( sockfd );
    return true;
}/*------------------------------------------------------------------*/


/*
 * Local Refresh without restart/hup
 * - Should delete folders and messages.
 * - Restore default config via CLI
 *   \param nr. users
 */
static bool sut_refreshLocalWarm(  )
{
#if 0
    delete_msgs(  ) {
        imap_open( host, port );
        imap_login( "user", "user" );
        imap_select( "inbox" );
        imap_store( "1:* +flags (\\delete)" );
        imap_expunge(  );
    }

    delete_folders(  ) {
        imap_open( host, port );
        imap_login( "user", "user" );
        struct folder fld = imap_list( "\"\"", "*" );
        for( i = fld.count; i > 0; i-- )
            imap_delete( fld.name[i] );

    }

    fori
        = 1;
    i <= nr_users;
    i++ delete_msgs(  ) {
        login select inbox store 1:*+flags \ deleted
            a expunge}
    delete_folders(  ) {
        list "" * fori
            = nr_lines;
        i > 0;
        i-- delete mbox}
        end
#endif
            return true;
}


/*
 * Remote Refresh without restart/hup
 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_refreshRemoteWarm(  )
{
    return true;
}/*------------------------------------------------------------------*/


extern int daemon_running;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sut_sigpipe(  int sig)
{
    daemon_running =0;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sut_sigint(  int sig)
{
    daemon_running =0;
}


