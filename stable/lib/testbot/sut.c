#include "config.h"
#include "sut.h"
#include <wait.h>
/*
 * Dont use any GETENV, or cfgal variables in this file
 */

extern struct config_s cfg ;

/*  Start SUT   */
int
sut_start( int test_type,
           int timeout, char* maillog,
           char *start, char* hostname, int port
          )
{
        printf( "* sutStart ...\n" );

        if( test_type == TEST_LOCAL ) {
                sut_startLocal( timeout, maillog, start );
                sleep( 1 );
                return TRUE ;
        }

        if( test_type == TEST_REMOTE ) {
                sut_startRemote( timeout, maillog, start, hostname, port ) ;
                sleep( 1 );
                return TRUE;
        }
        return TRUE;
}

static int
sut_startRemote(
                int timeout, char* maillog,
                char* start, char* hostname, int port)
{
        int sockfd = -1;
        char cmd[PATH_MAX] = { 0 } ;

        sockfd = sock_connectTo( hostname, port );
        sprintf(cmd, "START %d %s %s", timeout, maillog, start );
        sock_sendLine( sockfd, cmd );
        close(sockfd);
        return TRUE;
}


/**
 * \brief wait until is started
 * Will read from maillog file, in a manner simmilar to
 * tail -f, looks for SUCCESS: supervise ready, and INFO: ready
 */
static int
sut_startLocal(
                int timeout, char* maillog, char* start )
{
        int fd = 0, rc = 0 ;
        char buf[512] = { 0 };
        char *supRdyStr = "SUCCESS: supervise ready";
        char *rdyStr = "INFO: ready";
        int supRdy = FALSE;

        printf( "* LOG++++++++++\n" );
        fd = open( maillog, O_RDONLY );
        lseek(fd, 0, SEEK_END);
        rc = system( start );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                // ask for options
                return FALSE;
        }
        for( ;; ) {
                int i=0;
                memset( buf, '\0', 512 );
                i = read( fd, buf, sizeof( buf ) - 1 );
                if( i < 0 ) {
                        fprintf( stderr, "ERR: Cant read syslog '%s'\n", maillog );
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
                        if( supRdy == FALSE ) {
                                rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                                supRdy = rc;
                                if( rc )
                                        printf( "~~~~~~~Found SUPERVISER~~~~~~~\n" );
                        } else {
                                rc = str_search( buf, i, rdyStr, strlen(rdyStr) );
                                if( rc ) {
                                        printf( "~~~~~~~\n" );
                                        close( fd );
                                        return TRUE;
                                }
                        }
                }
        }
        close( fd );
        return TRUE;
}



/*-------------------------------*/
/*     STOP   */
int
sut_stop( int test_type,
          int timeout, char* maillog, char *stop,
          char* hostname, int port )
{
        printf( "* axiStop ...\n" );
        if( test_type == TEST_LOCAL ) {
                sut_stopLocal( timeout, maillog, stop );
                sleep(1);
        }
        if( test_type == TEST_REMOTE ) {
                sut_stopRemote( timeout, maillog, stop, hostname, port );
                sleep(1);
        }
        return 0;
}

static int
sut_stopRemote( int timeout,
                char* maillog, char* stop,
                char* hostname, int port)
{

        int sockfd = -1;
        char cmd[PATH_MAX] = { 0 } ;
        sockfd = sock_connectTo( hostname, port );
        sprintf(cmd, "STOP %d %s %s", timeout, maillog, stop );
        sock_sendLine( sockfd, cmd );
        close(sockfd);
        return TRUE ;
}


static int
sut_stopLocal( int timeout,
               char* maillog, char* stop )
{
        int fd = 0, rc = 0;
        char buf[512] = { 0 };
        char *supRdyStr = "INFO: supervise: caught signal";
        char *rdyStr = "INFO: supervise: finished";
        int supRdy = FALSE;

        printf( "* LOG\n++++++++++\n" );
        fd = open( maillog, O_RDONLY );
        lseek( fd, 0, SEEK_END );
        rc = system( stop );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                // ask for options
                return FALSE;
        }
        if( WEXITSTATUS(rc) == 1 ) {
                printf("No process killed: returning true\n");
                return TRUE ;
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
                        //sleep(1);
                        continue;
                } else {
                        buf[i] = '\0';
                        printf( "^^%s^^\n", buf );
                        /* Looking for a SUCCESS: supervise ready */
                        /* a possible bug might be if SUPERVISER text gets in the same
                         * buffer with INFO: ready. Then we get a false status*/
                        if( supRdy == FALSE ) {
                                rc = str_search( buf, i, supRdyStr, strlen(supRdyStr) );
                                supRdy = rc;
                                if( rc )
                                        printf( "////////Found SUPERVISER/////\n" );
                        } else {
                                rc = str_search( buf, i, rdyStr, strlen(rdyStr) );
                                if( rc ) {
                                        printf( "----------\n" );
                                        close( fd );
                                        return TRUE;
                                }
                        }
                }
        }
        return 1;
}



/*  REFRESH */
int
sut_refresh( int test_type,
             char *source, char *dest,
             char *host, int port )
{
        struct stat buf;
        int cod = 0;

        if( test_type == TEST_LOCAL ) {
                return sut_refreshLocal( source, dest );
        }

        if( test_type == TEST_REMOTE ) {
                if( sut_refreshRemote( host, port, source, dest ) ) {
                        printf( "* refresh: ERR: Could not make refresh!!!\n" );
                        return EXIT_FAILURE;
                }
                return 0;
        }
        return 0;
}





static int
sut_refreshLocal( char* source, char* dest )
{
        char cmd[8192] = { 0 } ;
        int cod ;
        struct stat buf ;
        printf( "* refresh: Removing %s\n", dest );
        sprintf( cmd, "/bin/rm -rf %s", dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: %s not deleted\n", dest );
                return FALSE;
        }
        cod = stat( source, &buf );
        if( cod != 0 ) {
                fprintf( stderr, "* refresh: ERR: %s: %s\n", source,
                            strerror( errno ) );
                exit( -1 );
        }
        printf( "* refresh: copying %s to %s\n", source, dest );
        if( dest[strlen( dest ) - 1] == '/' )
                dest[strlen( dest ) - 1] = '\0';
        sprintf( cmd, "/bin/cp -R %s %s", source, dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: File not copied\n" );
                return FALSE;
        }
        return TRUE;
}

static int
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
        return TRUE;
}



int
sut_checkCore( int test_type,
              const char *core_srcDir, const char *dbg_srcDir,
              const char *axi_workDir, const char *axi_cfgFile,
              const char *crash_destDir )
{
        int rc = FALSE;

        if( test_type == TEST_LOCAL ) {
                rc = sut_checkCoreLocal( core_srcDir, dbg_srcDir, axi_workDir,
                                          axi_cfgFile, crash_destDir );
            }

        if( test_type == TEST_REMOTE ) {
                rc = sut_checkCoreRemote( core_srcDir, dbg_srcDir, axi_workDir,
                                         axi_cfgFile, crash_destDir );
            }

        if( rc && !cfg.act_as_daemon )
                system( "echo -e 'Tested Product droped CORE!'|wall" );
        return rc;
}


static int
sut_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                    const char *axi_workDir, const char *axi_cfgFile,
                    const char *crash_destDir )
{
        char cmd[LINE_MAX] = { 0 };      // Max line should therefore be 3 times the length of PATH_MAX
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
                return TRUE;
        }
        return FALSE;
}




/**
 * \brief Check to see if a core was droped.
 * \return FALSE if core not dropped.
 * For the moment, this works local only.
 * Have to implement the same function in host.c*/
int
sut_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                     const char *workDir, const char *cfgFile,
                     const char *core_dstDir )
{
        DIR *dir;
        struct dirent *entry, *core;
        int rc = 0;
        char src[PATH_MAX] = { 0 };
        char dst[PATH_MAX] = { 0 };
        char cmd[2 * PATH_MAX + 32] = { 0 };

        if( !(dir = opendir( core_srcDir )) ) {
                fprintf( stderr, "1: Can't open core_srcDir [%s] : %s\n", core_srcDir,
                         strerror( errno ) );
                exit( -1 );
        }

        while( ( core = readdir( dir ) ) ) {
                if( strstr( core->d_name, "core" ) ) {
                        sprintf( src, "%s/%s"       , core_srcDir, core->d_name );
                        sprintf( dst, "%s/dumps/%s" , core_dstDir, core->d_name );
                        if( mkdir( dst, 0777 ) == -1 ) {
                                fprintf( stderr, "Can't create core folder %s : %s\n",
                                         dst, strerror(errno) );
                                return TRUE;
                        }

                        // 1. Move CORE
                        sprintf( cmd, "/bin/mv %s %s", src, dst );
                        system( cmd );
                        closedir( dir );

                        // 2. Move DEBUGLOGS
                        ;
                        if( !(dir = opendir( dbg_srcDir )) ) {
                                fprintf( stderr, "2: Can't open %s : %s\n",
                                         dbg_srcDir, strerror( errno ) );
                                return TRUE;
                        }
                        while( ( entry = readdir( dir ) ) ) {
                                if( !strcmp( entry->d_name, "." )
                                    || !strcmp( entry->d_name, ".." ) )
                                        continue;
                                sprintf( src, "%s/%s", dbg_srcDir, entry->d_name );
                                sprintf( cmd, "/bin/mv %s %s", src, dst );
                                system( cmd );
                        }
                        closedir( dir );

                        // 3. Move LOG
                        sprintf( src, "%s/log/default.txt", workDir );
                        sprintf( cmd, "/bin/mv %s %s", src, dst );
                        system( cmd );

                        // 4. copy  CONFIG
                        sprintf( cmd, "/bin/cp %s %s", cfgFile, dst );
                        system( cmd );
                        return TRUE;
                }
        }
        return FALSE;
}



void
sut_sigpipe(  )
{
        printf( "Bailing out\n" );
        exit( 1 );
}

void
sut_sigint()
{
        printf( "Bailing out\n" );
        exit( 1 );
}
