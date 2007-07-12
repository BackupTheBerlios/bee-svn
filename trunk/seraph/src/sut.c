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
#include "sut_private.h"
#include "core.h"

extern struct config_s cfg;
int sut_operation_timeout = 0;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_start( const int test_type,
           const int timeout, const char *maillog,
           const char *start, const char *hostname,const  int port )
{
    dbg_verbose( "START\n");

    if( test_type == TEST_LOCAL )
    {       sut_startLocal( timeout, maillog, start );
        return true;
    }

    if( test_type == TEST_REMOTE )
    {       sut_startRemote( timeout, maillog, start, hostname, port );
        return true;
    }
    return true;
}/*------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_startManager( const char* uName,
                  const char* host)
{
    char cmd[512]={0};
    int rc=0;
    sprintf( cmd, "ssh %s@%s 'mngrd -R 5000'", uName, host);
    dbg_verbose( "START-manager\n");
    rc = system(cmd);
    if( (rc ==-1) || WEXITSTATUS(rc) !=0) return false;
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
    bool ret;

    sockfd = sock_connectTo( hostname, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "START %d %s %s", timeout, maillog, start );
    sock_sendLine( sockfd, cmd );
    ret = sock_getStatus( sockfd );
    close( sockfd );
    if( ret < 0 )
    {   dbg_error("start: [%s]\n", strerror(ret) );
        return false;
    }
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
    char *finStr    = "INFO: supervise: finished";
    bool supRdy     = false;

    printf( "==> %s <==\n", maillog );
    fd = open( maillog, O_RDONLY );
    if( fd < 0 )
    {   dbg_error( "Can't read syslog [%s]: [%s]\n", maillog, strerror(errno) );
        return false;
    }

    if( -1 == fstat( fd, &stat_buf ) )
    {   dbg_error("Unable to stat [%s]: [%s]\n", maillog, strerror(errno) );
        close( fd ) ;
        return false;
    }
    st_ino = stat_buf.st_ino;
    lseek( fd, 0, SEEK_END );
    rc = system( start );
    if( rc == -1 )
    {   printf( "Failed\n" );
        exit( EXIT_FAILURE );
        /* TODO: ask for options */
    }
    /* 1 minute for SUT to start */
    {
        struct sigaction action;
        action.sa_handler = core_onSigAlrm;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction( SIGALRM, &action, NULL);
        alarm(1*60);
    }
    while( !sut_operation_timeout ) {
        int i = 0;
        memset( buf, '\0', 512 );
        if( -1 == fstat( fd, &stat_buf ) )
        {   dbg_error( "Unable to stat [%s]: [%s]\n", maillog, strerror(errno) );
            return false;
        }
        if( stat_buf.st_ino != st_ino )
        {   printf( "[%s] has been replaced;  following end of new file", maillog );
            close( fd );
            fd = open( maillog, O_RDONLY );
            if( -1 == fstat( fd, &stat_buf ) )
            {   dbg_error( "Unable to stat [%s]: [%s]\n", maillog, strerror(errno) );
                break;
            }
            st_ino = stat_buf.st_ino;
            lseek( fd, 0, SEEK_END );
        }
        i = read( fd, buf, sizeof( buf ) - 1 );
        if( i < 0 )
        {   dbg_error( "Can't read syslog [%s]: [%s]\n", maillog, strerror(errno) );
            break;
        }
        if( i == 0 )
        {   continue;
        } else
        {   buf[i] = '\0';
            printf( "%s", buf );
            /* Looking for a SUCCESS: supervise ready
             * a possible bug might be if SUPERVISER text gets in the same
             * buffer with INFO: ready. Then we get a false status*/
            if( !supRdy )
            {   supRdy = rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                if( rc )
                {   printf( "\n==> supervise ready <==\n" );
                    continue;
                }
            } else if( str_search(buf, i, rdyStr, strlen(rdyStr)) )
            {   printf( "\n==> server started <==\n" );
                close( fd );
                return true;
            } else if( str_search(buf, i, finStr, strlen(finStr)) )
            {   printf( "\n==> supervise: finished <==\n" );
                break;
            }
        }
    }
    sut_operation_timeout = 0;
    close( fd );
    return false;
}/*------------------------------------------------------------------*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_stop( const int test_type,
        const int timeout, const char *maillog, const char *stop, const char *hostname, const int port )
{
    dbg_verbose( "STOP\n");
    if( test_type == TEST_LOCAL ) {
        sut_stopLocal( timeout, maillog, stop );
        return true;
    }
    if( test_type == TEST_REMOTE ) {
        sut_stopRemote( timeout, maillog, stop, hostname, port );
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
    bool ret;

    sockfd = sock_connectTo( hostname, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "STOP %d %s %s", timeout, maillog, stop );
    sock_sendLine( sockfd, cmd );
    ret = sock_getStatus(sockfd);
    close( sockfd );
    if( ret < 0 )
    {   dbg_error("stop: [%s]\n", strerror(ret) );
        return false;
    }
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
    if( rc == -1 )
    {   printf( "Failed\n" );
        exit( EXIT_FAILURE );
        return false;
        /* TODO: ask for options */
    }

    if( WEXITSTATUS( rc ) == 1 )
    {   printf( "No process killed\n" );
        return true;
    }

    /* 1 minute for SUT to stop */
    {
        struct sigaction action;
        action.sa_handler = core_onSigAlrm;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction( SIGALRM, &action, NULL);
        alarm(1*60);
    }
    while( !sut_operation_timeout )
    {   int i = 0;
        memset( buf, '\0', 512 );
        i = read( fd, buf, sizeof( buf ) - 1 );
        if( i < 0 )
        {   dbg_error( "Can't read syslog [%s]: [%s]\n", maillog, strerror(errno) );
            break;
        }
        if( i == 0 )
        {   continue;
        } else
        {   buf[i] = '\0';
            printf( "%s", buf );
            /* Looking for a SUCCESS: supervise ready */
            /* a possible bug might be if SUPERVISER text gets in the same
             * buffer with INFO: ready. Then we get a false status*/
            if( !supRdy )
            {   supRdy = rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                if( rc )
                    printf( "\n==> supervise caught signal <==\n" );
            } else
            {   rc = str_search( buf, i, rdyStr, strlen(rdyStr) );
                if( rc )
                {   printf( "\n==> supervise finished <==\n" );
                    close( fd );
                    return true;
                }
            }
        }
    }
    sut_operation_timeout = 0;
    close( fd );
    return false;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool
sut_refresh( const int test_type, int refresh, const char* refreshCmd, const char *host, const int port )
{
    if( refresh == OPT_NO )
        return 0;
    dbg_verbose("Refreshing...\n");

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
    int ret, sockfd;

    dbg_verbose("Refresh\n");
    sockfd = sock_connectTo( host, port );
    if(sockfd==-1 )
    {
        return false;
    }

    sprintf( command, "REFRESH %s", refreshCmd);
    dbg_verbose("refresh [%s]\n", command);
    sock_sendLine( sockfd, command );
    ret = sock_getStatus( sockfd );
    close( sockfd );
    if( ret < 0 )
    {   dbg_error("refresh: [%s]\n", strerror(ret) );
        return false;
    }
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


