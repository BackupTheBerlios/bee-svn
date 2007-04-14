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
#include "sut.h"
#include "socket.h"
#include "strop.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>
#include "debug.h"
#include "rshd.h"
#include "strop.h"
#include "fileop.h"
#include "wall.h"
#include <glib.h>
/*#include <limits.h>*/
/*
 * Dont use any GETENV, or cfgal variables in this file
 */

extern struct config_s cfg;
int running ;

static bool sut_startRemote( const int timeout, const char *maillog, const char *start,
			     const char *host, const int port );

static bool sut_startLocal( const int timeout, const char *maillog, const char *start );

static bool sut_stopRemote( const int timeout, const char *maillog, const char *stop,
			    const char *host, const int port );

static bool sut_stopLocal( const int timeout, const char *maillog, const char *stop );

static bool sut_refreshRemote( const char *host, const int port, const char *source, const char *dest );

static bool sut_refreshLocal( const char *source, const char *dest );

static bool
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
		     const char *axi_workDir, const char *axi_cfgFile,
		     const char *crash_destDir );
static bool
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
		    const char *workDir, const char *cfgFile,
		    const char *crash_destDir );

static int sut_runBat( const char *bat_name, int timeout );

static int sut_setupTmp( char const *source_bat, char *tmpDir );

static int sut_parseBat( const char *filename );

static int sut_dirAction( const char *fileName, struct stat *statbuf,
			  void *junk );

static int sut_fileAction( const char *fileName, struct stat *statbuf,
			   void *junk );

static int sut_sutRefresh( const int option, const char *filename );

static int sut_cleanupTmp( const char *tmpDir );

static int sut_setErrorlog( void );

static int sut_runRecursive( const char *srcName );



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
    char**
sut_listTests(const char* td, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;

    char** buf =(char**)calloc(8192, sizeof(char*)); //TODO
    if( !( dir = opendir( td ) ) ) {
        debug( "1: Can't open test directory [%s] : %s\n",
                td, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    while( ( ent = readdir( dir ) ) ) {
        if( !strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..") ||!strcmp(ent->d_name, "CVS") )
            continue;
        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    return buf;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
sut_listMachines(const char* td, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;
    char** buf;

    debug("LISTMACHINES: [%s]\n", td);
    buf =(char**)calloc(8192, sizeof(char*)); /*TODO use linked list */

    if( !( dir = opendir(td) ) ) {
        debug( "1: Can't open directory [%s] : %s\n",
                td, strerror( errno ) );
        *sz = 0;
        return NULL ;
    }
    while( ( ent = readdir( dir ) ) ) {
        if( !strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..") ) continue;
        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    return buf;
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
sut_refresh( const int test_type, const char *source, const char *dest, const char *host, const int port )
{
    if( test_type == TEST_LOCAL ) {
        return sut_refreshLocal( source, dest );
    }

    if( test_type == TEST_REMOTE ) {
        if( !sut_refreshRemote( host, port, source, dest ) ) {
            printf( "* refresh: ERR: Could not make refresh!!!\n" );
            return false;
        }
        return true;
    }
    return true;
}/*------------------------------------------------------------------*/



static bool sut_refreshLocal( const char *source, const char *dest )
{
    char cmd[8192] = { 0 };
    int cod;
    struct stat buf;
    printf( "* refresh: Removing %s\n", dest );
    sprintf( cmd, "/bin/rm -rf %s", dest );
    if( system( cmd ) ) {
        printf( "! refresh: [%s] not deleted\n", dest );
        return false;
    }
    cod = stat( source, &buf );
    if( cod != 0 ) {
        fprintf( stderr, "* refresh: ERR: %s: %s\n", source,
                strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    printf( "* refresh: copying %s to %s\n", source, dest );
    sprintf( cmd, "/bin/cp -R %s %s/", source, dest );
    if( system( cmd ) ) {
        printf( "* refresh: ERR: File not copied\n" );
        return false;
    }
    return true;
}/*------------------------------------------------------------------*/



static bool sut_refreshRemote( const char *host, const int port, const char *sursa, const char *dest )
{
    char command[LINE_MAX] = { 0 };
    int cod, sockfd;

    sockfd = sock_connectTo( host, port );

    sprintf( command, "REFRESH %s %s", sursa, dest );
    sock_sendLine( sockfd, command );
    cod = sock_getStatus( sockfd );
    if( cod < 0 ) {
        fprintf( stderr, "ERR: %s\n", strerror( cod ) );
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



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
sut_checkCore( const int test_type,
        const char *core_srcDir, const char *dbg_srcDir,
        const char *axi_workDir, const char *axi_cfgFile,
        const char *crash_destDir )
{
    if( test_type == TEST_LOCAL )
        return sut_checkCoreLocal( core_srcDir, dbg_srcDir,
                axi_workDir, axi_cfgFile,
                crash_destDir );

    if( test_type == TEST_REMOTE )
        return sut_checkCoreRemote( core_srcDir, dbg_srcDir,
                axi_workDir, axi_cfgFile,
                crash_destDir );
    return false;
}/*------------------------------------------------------------------*/



    static bool
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
        const char *axi_workDir, const char *axi_cfgFile,
        const char *crash_destDir )
{
    char cmd[LINE_MAX] = { 0 };     /* Max line should therefore be 3 times the length of FILENAME_MAX */
    int rc = 0;
    int sock;

    sock = sock_connectTo( cfg.hostname, cfg.port );
    sprintf( cmd, "CHECKCORE %s %s %s %s %s",
            core_srcDir, dbg_srcDir, axi_workDir,
            axi_cfgFile, crash_destDir );
    sock_sendLine( sock, cmd );
    rc = sock_getStatus( sock );
    close( sock );
    if( rc ) {
        return true;
    }
    return false;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static bool
sut_setupDstDir( char *dst, char *coreName,
        const char *core_srcDir, const char *core_dstDir )
{
    sprintf( dst, "%s/dumps/%s-%s", core_dstDir, cfg.cur_test, coreName );
    if( mkdir( dst, 0777 ) == -1 ) {
        fprintf( stderr,
                "! seraph: Can't create coreDstDir %s : %s\n",
                dst, strerror( errno ) );
        return false;
    }
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_moveCore( char *src, char *dst )
{
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    int status;

    sprintf( cmd, "/bin/mv %s %s", src, dst );
    status = system( cmd );
    if( -1 == status || 0 != WEXITSTATUS( status ) ) {
        fprintf( stderr, "! sut: Unable to move [%s] to [%s]: [%s]\n",
                src, dst, strerror( errno ) );
        return false;
    }
    return true;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_moveDebugs( const char *srcDir, char *dst )
{
    DIR *dir;
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    struct dirent *entry;

    if( !( dir = opendir( srcDir ) ) ) {
        fprintf( stderr,
                "! seraph: Can't open dbgSrcDir [%s] : %s\n",
                srcDir, strerror( errno ) );
        return false;
    }
    while( ( entry = readdir( dir ) ) ) {
        if( !strcmp( entry->d_name, "." )
                || !strcmp( entry->d_name, ".." ) )
            continue;
        sprintf( src, "%s/%s", srcDir, entry->d_name );
        sprintf( cmd, "/bin/mv %s %s", src, dst );
        system( cmd );
    }
    if( -1 == closedir( dir ) ) {
        fprintf( stderr, "Unable to close dir [%s]: [%s]\n",
                srcDir, strerror( errno ) );
        return false;
    }
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_moveLog( const char *workDir, char *dst )
{
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( src, "%s/log/default.txt", workDir );
    sprintf( cmd, "/bin/mv %s %s", src, dst );
    system( cmd );
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool sut_copyCfg( const char *cfgFile, char *dst )
{
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( cmd, "/bin/cp %s %s", cfgFile, dst );
    system( cmd );
    return true;
}

/**
 * \brief Check to see if a core was droped.
 * \return false if core not dropped.
 * For the moment, this works local only.
 * Have to implement the same function in host.c*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
        const char *workDir, const char *cfgFile,
        const char *core_dstDir )
{
    DIR *dir;
    struct dirent *core;
    char src[FILENAME_MAX] = { 0 };
    char dst[FILENAME_MAX] = { 0 };

    if( !( dir = opendir( core_srcDir ) ) ) {
        fprintf( stderr, "1: Can't open core_srcDir [%s] : %s\n",
                core_srcDir, strerror( errno ) );
        exit( EXIT_FAILURE );
    }

    while( ( core = readdir( dir ) ) ) {
        if( strstr( core->d_name, "core" ) ) {
            bool rc = false;
            sprintf( src, "%s/%s", core_srcDir, core->d_name );

            rc = sut_setupDstDir( dst, core->d_name,
                    core_srcDir, core_dstDir );
            if( !rc )
                return false;

            sut_moveCore( src, dst );
            sut_moveDebugs( dbg_srcDir, dst );
            sut_moveLog( workDir, dst );
            sut_copyCfg( cfgFile, dst );

            return true;
        }
    }
    if( -1 == closedir( dir ) ) {
        fprintf( stderr, "Unable to close dir [%s]: [%s]\n",
                core_srcDir, strerror( errno ) );
        return false;
    }
    return false;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sut_sigpipe(  int sig)
{
    printf( "Bailing out\n" );
    exit( EXIT_SUCCESS );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sut_sigint(  int sig)
{
    printf( "Bailing out\n" );
    exit( EXIT_SUCCESS );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sut_sigterm(  int sig)
{
    printf( "SigTerm\n" );
    running = 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sig_handler( int sig )
{
    int status;
    switch ( sig ) {
        case SIGCHLD:
            while( waitpid( -1, &status, WNOHANG ) > 0 )
                if( WEXITSTATUS( status ) == 69 )
                    printf( "* seraph: PASS\n" );
                else
                    printf( "* seraph: FAIL [%d]\n", status );
            break;
        case SIGALRM:
            fprintf( stderr, "timeout\n" );
            break;
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_runBat( const char *bat_name, int timeout )
{
    char c;
    int pid = 0, hasAlarm;
    struct sigaction act;
    printf( "\n\n" );
    printf( "*-------------------------.\n" );
    printf( "* seraph: Running script : [%s]\n", bat_name );
    printf( "*-------------------------'\n" );

    act.sa_handler = sig_handler;
    act.sa_flags = SA_NOCLDSTOP;
    sigaction( SIGCHLD, &act, 0 );

    if( ( pid = fork(  ) ) == 0 ) {
        int rc = 0;
        rc = system( bat_name );
        exit( rc );
    } else if( pid > 0 ) {
        while( 1 ) {
            sleep( cfg.script_tout );
            if( waitpid( -1, &hasAlarm, WNOHANG ) )
                break;
            if( cfg.allways_kill ) {
                kill( pid, 9 );
                break;
            }

            do {
                printf( "* seraph: KILL Child?  [y/n]: " );
                c = getchar(  );
                while( getchar(  ) != '\n' );
            } while( c != 'y' && c != 'n' );
            if( c == 'y' ) {
                if( -1 == kill( pid, 9 ) ) {
                    perror
                        ( "! seraph: Unable to kill child script" );
                }
                break;
            }
        }
    } else
        perror( "! seraph: fork() failed" );
    return 0;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static int
sut_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
    char curDir[FILENAME_MAX] = { 0 };
    char tmpDir[FILENAME_MAX] = { 0 };
    char bn[32] = { 0 };
    char *fn_dup = 0;
    char *len = 0;

    fn_dup = strdup( fileName );
    len = strncpy( bn, basename( fn_dup ), 31 );
    free( fn_dup );

    if( strcmp( bn, "runtest.bat" ) || access( fileName, X_OK ) )
        return TRUE;
    sut_sutRefresh( cfg.refresh, fileName );
    sut_setupTmp( fileName, tmpDir );
    if( NULL == getcwd( curDir, FILENAME_MAX ) ) {
        perror( "! seraph: Unable to get current directory" );
    }

    if( -1 == chdir( tmpDir ) ) {
        fprintf( stderr,
                "! seraph: Unable to change directory to [%s]:[%s]\n",
                tmpDir, strerror( errno ) );
        return FALSE;
    }
    sut_runBat( fileName, cfg.script_tout );
    sut_checkCore( cfg.test_type, cfg.axi_coreDir, cfg.axi_dbgDir,
            cfg.axi_workDir, cfg.axi_cfgFile, cfg.dest_coreDir );
    sut_cleanupTmp( tmpDir );
    sleep( 1 );
    if( -1 == chdir( curDir ) ) {
        fprintf( stderr,
                "! seraph: Unable to change directory to [%s]:[%s]\n",
                curDir, strerror( errno ) );
        return FALSE;
    }

    return ( TRUE );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static int
sut_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
    return ( TRUE );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_runRecursive( const char *srcName )
{
    if( recursiveAction( srcName, 1, FALSE,
                TRUE, sut_fileAction, sut_dirAction,
                NULL ) == FALSE ) {
        printf( "error in runRecursive\n" );
        exit( EXIT_FAILURE );
    }
    return TRUE;
}



/*!
 * Copies source_bat in /tmp/PID, to create a sandbox for the test.
 * \param source_file[in] An absolute path pointing to a bat file.
 * \todo  Implement a sut_refresh function
 * \todo  Return Bool ??
 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_setupTmp( char const *source_bat, char *tmpDir )
{
    char cmd[FILENAME_MAX] = { 0 };
    int status;
    char *s = strdup( source_bat );

    sprintf( tmpDir, "/tmp/%d", getpid(  ) );
    if( -1 == mkdir( tmpDir, 0755 ) ) {
        fprintf( stderr,
                "! seraph: Can't make directory [%s]: [%s]\n",
                tmpDir, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    if( cfg.verbose == TRUE )
        printf( "# seraph: Copying [%s] to [%s]\n", s, tmpDir );
    sprintf( cmd, "/bin/cp -R %s/* %s/", dirname( s ), tmpDir );

    status = system( cmd );
    free( s );
    s = strdup( source_bat );
    if( -1 == status || 0 != WEXITSTATUS( status ) ) {
        fprintf( stderr,
                "! seraph: Unable to copy [%s] to [%s]: [%s]\n",
                dirname( s ), tmpDir, strerror( errno ) );
    }
    free( s );
    return 0;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_cleanupTmp( const char *tmpDir )
{
    printf( "# seraph: Removing [%s]\n", tmpDir );
    fop_rm( TEST_LOCAL, tmpDir, 0, 0 );
    return 0;
}



/** Search a file for axi_fi=y or axi_fi=n . */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_parseBat( const char *filename )
{
#if 0
    char line[LINE_MAX] = { 0 };
    char *str = 0;
    pcre *re = NULL;
    int erroffset;
    int ovector[30];
    int matches;
    const char *error = NULL;
    struct stat inf;
    FILE *f = NULL;
    re = pcre_compile( "#.*axi_fi\\s*=\\s*\"?([y|Y|n|N])\"?\\s+",
            0, &error, &erroffset, NULL );
    if( !re ) {
        fprintf( stderr, "Can't compile regular expression\n" );
        exit( EXIT_FAILURE );
    }
    if( -1 == stat( filename, &inf ) ) {
        fprintf( stderr, "! seraph: Unable to stat [%s]: [%s]\n",
                filename, strerror( errno ) );
        return OPT_YES;
    }
    if( ( inf.st_mode & S_IFMT ) == S_IFDIR ) {
        printf( "! seraph: [%s] is a directory\n", filename );
        exit( EXIT_FAILURE );
    }

    f = fopen( filename, "r" );
    if( !f ) {
        fprintf( stderr,
                "! seraph: [%d] Can't open file [%s] : [%s]\n",
                __LINE__, filename, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    printf( "# seraph: Scanning [%s]\n", filename );
    while( ( str = fgets( line, LINE_MAX - 1, f ) ) ) {
        ;
        matches =
            pcre_exec( re, NULL, str, strlen( str ), 0, 0, ovector,
                    30 );
        if( matches >= 2 ) {
            if( tolower( str[ovector[2]] ) == 'y' ) {
                fclose( f );
                return OPT_YES;
            }
            if( tolower( str[ovector[2]] ) == 'n' ) {
                fclose( f );
                return OPT_NO;
            }
        }
    }
    fclose( f );
    return OPT_YES;         /* If nothing is found, return the default
                               which is OPT_YES. */
#endif
    return 0;
}



/*
 * Restore the default configuration of the SUT.
 * SUT = server under test */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_sutRefresh( int refresh, const char *bat_file )
{
    int cod;
    int rc = 0;

    if( refresh == OPT_NO )
        return 0;

    if( refresh == OPT_ASK ) {
        char r = 0;

        do {
            printf
                ( "* seraph: Reinstall default configuration? [y/n]: " );
            r = getchar(  );
            while( getchar(  ) != '\n' );
        } while( r != 'y' && r != 'n' );
        if( r == 'n' ) {
            printf( "# seraph: Keeping current state\n" );
            return 0;
        }
    }

    cod = sut_parseBat( bat_file );
    if( cod == OPT_NO )
        return 0;

    /* We excluded all NO's, so we're left with the yes. */
    /* Try to restore the default ptgen state --groleo */
    debug("refresh\n");
    rc = system( "refresh" );
    debug( "# seraph: refresh retVal : %d\n", rc );
    if( rc == -1 || WEXITSTATUS( rc ) != 0 ) {
        debug( "! seraph: Can't make refresh!\n" );
        exit( EXIT_FAILURE );
    }

    return 0;
}



/**
 * \brief search and expand any occurence in str, of ${VARNAME} */
/* expand ${VAR} from prefix/${VAR}/suffix */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char* sut_expandVars( const char *t1 )
{
    char *p = 0, *s = 0, *val;
    int i = 0;
    char id[512] = { 0 };
    int sz_new = 0, sz_old = 1024;

    char *text = (char*)calloc( sz_old, sizeof( char ) );

    strncpy( text, t1, 1023 );

    for( p = text; *p != '\0'; p++ ) {
        /*printf("[%c][%d]\n", *p, *p ) ; */
        if( *p == '$' ) {
            s = p++;        /* saving position of $ */
            /*printf("[%c][%d]\n", *p, *p ) ; */
            if( *p != '{' ) {
                printf( "Error expanding %s\n", text );
                return 0;
            }

            p++;
            /*printf("[%c][%d]\n", *p, *p ) ; */
            while( *p != '}' && *p != '\0' && i < 512 ) {
                id[i++] = *p;
                p++;
                /*printf("[%c][%d]\n", *p, *p ) ; */
            }
            if( *p == '\0' ) {
                printf( "Unmatched bracket in %s\n", text );
                return 0;
            }
            p++;
            if( NULL == getenv( id ) ) {
                debug( "Can't expand [%s] : Not exported\n", id );
                memset( id, 0, 512 );
                i = 0;  /* Reset ID pointer and ID size counter */
                continue;
            }
            val = strdup( p );
            /* compute the length of the final string */
            sz_new =
                strlen( p ) + strlen( getenv( id ) ) +
                strlen( val );
            if( sz_new > sz_old ) {
                text = (char*)realloc( text, sz_new );
                sz_old = sz_new;
            }
            strcpy( s, getenv( id ) );      /* Push in the expansion */
            strcpy( s + strlen( getenv( id ) ), val );      /*Copy suffix */
            free( val );
            p += strlen( getenv( id ) ) - strlen( id ) - 3; /* Advance the parsing pointer */
            memset( id, 0, 512 );
            i = 0;  /* Reset ID pointer and ID size counter */
        }
    }
    return text;
}



/*
 * Export axi_errorlog=$CWD/errors/$HOST-$PID
 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sut_setErrorlog(  )
{
    char rez[LINE_MAX] = "";
    struct tm *t = 0;
    time_t now;
    char dn[PATH_MAX]={0}, *p=0;

    time( &now );
    t = localtime( &now );
    sprintf( rez, "%s/errors/%s-%d-%02d:%02d:%02d",
            cfg.seraph_path, cfg.hostname, getpid(  ),
            t->tm_hour, t->tm_min, t->tm_sec );

    printf( "# seraph: Log is: [%s]\n", rez );
    setenv( SUT_ERRLOG, rez, 1 );

    /* Create ./errors in case it doesn't exists */
    strcpy(dn,rez);
    p = dirname(dn);
    if( -1 == access( p, F_OK ) ) {
        fprintf( stderr, "%s doesn't exist\n", dirname( rez ) );
        if( -1 == mkdir( p, 0777 ) ) {
            fprintf( stderr, "SetErrorLog: Unable to create [%s]: [%s]\n",
                    p,strerror( errno ) );
            exit( EXIT_FAILURE );
        }
        fprintf( stderr, "%s created\n", p );
    }

    /* If it exists, but we dont have write permissions */
    if( -1 == access( p, W_OK ) ) {
        fprintf( stderr, "%s doesn't have write permissions\n", p );
        exit( EXIT_FAILURE );
    }
    return 0;
}




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int sut_runTests( const char *dir )
{
    struct stat inf;
    char fullPath[FILENAME_MAX] = { 0 };
    char curDir[FILENAME_MAX] = { 0 };

    if( stat( dir, &inf ) < 0 ) {
        printf( "! seraph: The directory [%s] doesn't exist.\n", dir );
        return 0;
    }
    getcwd( curDir, FILENAME_MAX );
    if( -1 == chdir( dir ) ) {
        fprintf( stderr, "! seraph: Can't change to [%s] : [%s]\n",
                dir, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    /*! @todo verify if `tests` dir is an absolute or a relative path */
    sprintf( fullPath, "%s/%s", curDir, dir );
    //        sut_runRecursive( fullPath );
    return TRUE;
}

int onLineParsed(const char *name, const char *value, void* arg)
{
    GSList** a = (GSList**)arg;
    ConfigEntry* el = calloc(1, sizeof(ConfigEntry));

    printf("%s -- %s\n", name, value);
    el->symbol = calloc(1, strlen(name)+1);
    el->value = calloc(1, strlen(value)+1);

    strcpy(el->symbol, name);
    strcpy(el->value, value);
    *a = g_slist_append (*a, el);
    return 0;
}


GSList* sut_getConfig(const char* machine, int* sz)
{
    char path[PATH_MAX]={0};
    debug( "\nGETCONFIG\n" );
    GSList*   cfgTable =  g_slist_alloc() ;
    cfg.takeAction = onLineParsed ;
    sprintf(path,"%s/%s", MACHINES, machine);
    srph_parseCfg( path, &cfgTable);
    *sz = g_slist_length(cfgTable);
    return cfgTable;
}
