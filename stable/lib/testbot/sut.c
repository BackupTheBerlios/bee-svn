#include "config.h"
#include "sut.h"
#include "socket.h"
#include "strop.h"
#include <wait.h>
#include <limits.h>
/*
 * Dont use any GETENV, or cfgal variables in this file
 */

extern struct config_s cfg ;
static bool sut_startRemote( int timeout, char* maillog, char* start, char* host, int port);
static bool sut_startLocal( int timeout, char* maillog, char* start);
static bool sut_stopRemote( int timeout, char* maillog, char* stop, char* host, int port);
static bool sut_stopLocal( int timeout, char* maillog, char* stop);
static bool sut_refreshRemote( char* host, int port, char* source, char* dest);
static bool sut_refreshLocal( char* source, char* dest);
static bool
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                    const char *axi_workDir, const char *axi_cfgFile,
                    const char *crash_destDir );
static bool
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                     const char *workDir, const char *cfgFile,
                     const char *crash_destDir );

/*  Start SUT   */
bool
sut_start( int test_type,
           int timeout, char* maillog,
           char *start, char* hostname, int port
          )
{
        printf( "* sutStart ...\n" );

        if( test_type == TEST_LOCAL ) {
                sut_startLocal( timeout, maillog, start );
                sleep( 1 );
                return true ;
        }

        if( test_type == TEST_REMOTE ) {
                sut_startRemote( timeout, maillog, start, hostname, port ) ;
                sleep( 1 );
                return true ;
        }
        return true;
}


/*
 * \todo Test for failure */
static bool
sut_startRemote(
                int timeout, char* maillog,
                char* start, char* hostname, int port)
{
        int sockfd = -1;
        char cmd[FILENAME_MAX] = { 0 } ;

        sockfd = sock_connectTo( hostname, port );
        sprintf(cmd, "START %d %s %s", timeout, maillog, start );
        sock_sendLine( sockfd, cmd );
        close(sockfd);
        return true;
}


/**
 * \brief wait until is started
 * Will read from maillog file, in a manner simmilar to
 * tail -f, looks for SUCCESS: supervise ready, and INFO: ready
 */
static bool
sut_startLocal(
                int timeout, char* maillog, char* start )
{
        int fd = 0, rc = 0 ;
        char buf[512] = { 0 };
        char *supRdyStr = "SUCCESS: supervise ready";
        char *rdyStr = "INFO: ready";
        bool supRdy = false ;

        printf( "* LOG++++++++++\n" );
        fd = open( maillog, O_RDONLY );
        lseek(fd, 0, SEEK_END);
        rc = system( start );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                /* ask for options */
                return false ;
        }
        for( ;; ) {
                int i=0;
                memset( buf, '\0', 512 );
                i = read( fd, buf, sizeof( buf ) - 1 );
                if( i < 0 ) {
                        fprintf( stderr, "! testbot: Cant read syslog '%s'\n", maillog );
                        break;
                }
                if( i == 0 ) {
                        continue;
                } else {
                        buf[i] = '\0';
                        printf( "^^%s^^\n", buf );
                        /* Looking for a SUCCESS: supervise ready */
                        /* a possible bug might be if SUPERVISER text gets in the same
                         * buffer with INFO: ready. Then we get a false status*/
                        if( !supRdy ) {
                                rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                                supRdy = rc;
                                if( rc )
                                        printf( "~~~~~~supervise ready~~~~~~\n" );
                        } else {
                                rc = str_search( buf, i, rdyStr, strlen(rdyStr) );
                                if( rc ) {
                                        printf( "~~~~~~server started~~~~~~\n" );
                                        close( fd );
                                        return true ;
                                }
                        }
                }
        }
        close( fd );
        return true ;
}



/*-------------------------------*/
/*     STOP   */
bool
sut_stop( int test_type,
          int timeout, char* maillog, char *stop,
          char* hostname, int port )
{
        printf( "* axiStop ...\n" );
        if( test_type == TEST_LOCAL ) {
                sut_stopLocal( timeout, maillog, stop );
                sleep(1);
                return true ;
        }
        if( test_type == TEST_REMOTE ) {
                sut_stopRemote( timeout, maillog, stop, hostname, port );
                sleep(1);
                return true ;
        }
        return true ;
}


/*
 * \todo Test for failure */
static bool
sut_stopRemote( int timeout,
                char* maillog, char* stop,
                char* hostname, int port)
{

        int sockfd = -1;
        char cmd[FILENAME_MAX] = { 0 } ;
        sockfd = sock_connectTo( hostname, port );
        sprintf(cmd, "STOP %d %s %s", timeout, maillog, stop );
        sock_sendLine( sockfd, cmd );
        close(sockfd);
        return true ;
}


static bool
sut_stopLocal( int timeout,
               char* maillog, char* stop )
{
        int fd = 0, rc = 0;
        char buf[512] = { 0 };
        char *supRdyStr = "INFO: supervise: caught signal";
        char *rdyStr = "INFO: supervise: finished";
        bool supRdy = false ;

        printf( "* LOG\n++++++++++\n" );
        fd = open( maillog, O_RDONLY );
        lseek( fd, 0, SEEK_END );
        rc = system( stop );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                /* ask for options */
                return false ;
        }
        if( WEXITSTATUS(rc) == 1 ) {
                printf("No process killed\n");
                return true ;
        }
        for( ;; ) {
                int i =0 ;
                memset( buf, '\0', 512 );
                i = read( fd, buf, sizeof( buf ) - 1 );
                if( i < 0 ) {
                        fprintf( stderr, "Cant read syslog '%s'\n", maillog );
                        break;
                }
                if( i == 0 ) {
                        /* sleep(1); */
                        continue;
                } else {
                        buf[i] = '\0';
                        printf( "^^%s^^\n", buf );
                        /* Looking for a SUCCESS: supervise ready */
                        /* a possible bug might be if SUPERVISER text gets in the same
                         * buffer with INFO: ready. Then we get a false status*/
                        if( !supRdy ) {
                                rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                                supRdy = rc;
                                if( rc )
                                        printf( "~~~~~~supervise caugth signal~~~~~~\n" );
                        } else {
                                rc = str_search( buf, i, rdyStr, strlen(rdyStr) );
                                if( rc ) {
                                        printf( "~~~~~~supervise finished~~~~~~\n" );
                                        close( fd );
                                        return true ;
                                }
                        }
                }
        }
        return true ;
}



/*  REFRESH */
bool
sut_refresh( int test_type,
             char *source, char *dest,
             char *host, int port )
{
        if( test_type == TEST_LOCAL ) {
                return sut_refreshLocal( source, dest );
        }

        if( test_type == TEST_REMOTE ) {
                if( !sut_refreshRemote( host, port, source, dest ) ) {
                        printf( "* refresh: ERR: Could not make refresh!!!\n" );
                        return false ;
                }
                return true ;
        }
        return true ;
}





static bool
sut_refreshLocal( char* source, char* dest )
{
        char cmd[8192] = { 0 } ;
        int cod ;
        struct stat buf ;
        printf( "* refresh: Removing %s\n", dest );
        sprintf( cmd, "/bin/rm -rf %s", dest );
        if( system( cmd ) ) {
                printf( "! refresh: [%s] not deleted\n", dest );
                return false ;
        }
        cod = stat( source, &buf );
        if( cod != 0 ) {
                fprintf( stderr, "* refresh: ERR: %s: %s\n", source,
                            strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        printf( "* refresh: copying %s to %s\n", source, dest );
        if( dest[strlen( dest ) - 1] == '/' )
                dest[strlen( dest ) - 1] = '\0';
        sprintf( cmd, "/bin/cp -R %s %s", source, dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: File not copied\n" );
                return false;
        }
        return true;
}

static bool
sut_refreshRemote( char *host, int port, char *sursa, char *dest )
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
}



bool
sut_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir )
{
        if( test_type == TEST_LOCAL )
                return sut_checkCoreLocal( core_srcDir, dbg_srcDir, axi_workDir,
                                          axi_cfgFile, crash_destDir );

        if( test_type == TEST_REMOTE )
                return sut_checkCoreRemote( core_srcDir, dbg_srcDir, axi_workDir,
                                         axi_cfgFile, crash_destDir );
        return false ;
}


static bool
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                    const char *axi_workDir, const char *axi_cfgFile,
                    const char *crash_destDir )
{
        char cmd[LINE_MAX] = { 0 };      /* Max line should therefore be 3 times the length of FILENAME_MAX */
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
}


/*---------------*/
static bool
sut_setupDstDir( char* dst, char* coreName,
                 const char* core_srcDir, const char* core_dstDir)
{
    sprintf( dst, "%s/dumps/%s-%s" , core_dstDir, cfg.cur_test, coreName );
    if( mkdir( dst, 0777 ) == -1 ) {
        fprintf( stderr, "! testbot: Can't create coreDstDir %s : %s\n",
                dst, strerror(errno) );
        return false;
    }
    return true ;
}

static bool
sut_moveCore( char* src, char* dst )
{
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( cmd, "/bin/mv %s %s", src, dst );
    system( cmd );
    return true ;
}

static bool
sut_moveDebugs( const char* srcDir, char* dst )
{
    DIR *dir;
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    struct dirent *entry;

    if( !(dir = opendir( srcDir )) ) {
        fprintf( stderr, "! testbot: Can't open dbgSrcDir [%s] : %s\n",
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
    closedir( dir );
    return true ;
}

static bool
sut_moveLog( const char* workDir, char* dst )
{
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( src, "%s/log/default.txt", workDir );
    sprintf( cmd, "/bin/mv %s %s", src, dst );
    system( cmd );
    return true ;
}
static bool
sut_copyCfg( const char* cfgFile, char* dst )
{
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( cmd, "/bin/cp %s %s", cfgFile, dst );
    system( cmd );
    return true ;
}
/**
 * \brief Check to see if a core was droped.
 * \return false if core not dropped.
 * For the moment, this works local only.
 * Have to implement the same function in host.c*/
bool
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                    const char *workDir, const char *cfgFile,
                    const char *core_dstDir )
{
        DIR *dir;
        struct dirent *core;
        char src[FILENAME_MAX] = { 0 };
        char dst[FILENAME_MAX] = { 0 };

        if( !(dir = opendir( core_srcDir )) ) {
                fprintf( stderr, "1: Can't open core_srcDir [%s] : %s\n", core_srcDir,
                         strerror( errno ) );
                exit( EXIT_FAILURE );
        }

        while( ( core = readdir( dir ) ) ) {
                if( strstr( core->d_name, "core" ) ) {
                        bool rc = false ;
                        sprintf( src, "%s/%s" , core_srcDir, core->d_name );

                        rc = sut_setupDstDir( dst, core->d_name, core_srcDir, core_dstDir);
                        if( !rc ) return false ;

                        sut_moveCore( src, dst ) ;
                        sut_moveDebugs( dbg_srcDir, dst );
                        sut_moveLog( workDir, dst ) ;
                        sut_copyCfg( cfgFile, dst ) ;

                        return true;
                }
        }
        closedir( dir );
        return false;
}



void
sut_sigpipe(  )
{
        printf( "Bailing out\n" );
        exit( EXIT_SUCCESS );
}

void
sut_sigint()
{
        printf( "Bailing out\n" );
        exit( EXIT_SUCCESS );
}
