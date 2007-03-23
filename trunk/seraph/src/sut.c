#include "config.h"
#include "sut.h"
#include "socket.h"
#include "strop.h"
#include <sys/wait.h>
#include <sys/types.h>
/*#include <limits.h>*/
/*
 * Dont use any GETENV, or cfgal variables in this file
 */

extern struct config_s cfg;
static bool sut_startRemote( int timeout, char *maillog, char *start,
                             char *host, int port );
static bool sut_startLocal( int timeout, char *maillog, char *start );
static bool sut_stopRemote( int timeout, char *maillog, char *stop,
                            char *host, int port );
static bool sut_stopLocal( int timeout, char *maillog, char *stop );
static bool sut_refreshRemote( char *host, int port, char *source, char *dest );
static bool sut_refreshLocal( char *source, char *dest );
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
           int timeout, char *maillog, char *start, char *hostname, int port )
{
        printf( "* sutStart ...\n" );

        if( test_type == TEST_LOCAL ) {
                sut_startLocal( timeout, maillog, start );
                sleep( 1 );
                return true;
        }

        if( test_type == TEST_REMOTE ) {
                sut_startRemote( timeout, maillog, start, hostname, port );
                sleep( 1 );
                return true;
        }
        return true;
}


/*
 * \todo Test for failure */
static bool
sut_startRemote( int timeout, char *maillog,
                 char *start, char *hostname, int port )
{
        int sockfd = -1;
        char cmd[FILENAME_MAX] = { 0 };

        sockfd = sock_connectTo( hostname, port );
        sprintf( cmd, "START %d %s %s", timeout, maillog, start );
        sock_sendLine( sockfd, cmd );
        close( sockfd );
        return true;
}


/**
 * \brief wait until is started
 * Will read from maillog file, in a manner simmilar to
 * tail -f, looks for SUCCESS: supervise ready, and INFO: ready
 */
static bool sut_startLocal( int timeout, char *maillog, char *start )
{
        int fd = 0, rc = 0;
        ino_t st_ino;
        struct stat stat_buf;
        char buf[512] = { 0 };
        char *supRdyStr = "SUCCESS: supervise ready";
        char *rdyStr = "INFO: ready";
        bool supRdy = false;

        printf( "==> %s <==\n", maillog );
        fd = open( maillog, O_RDONLY );
        if( fd < 0 ) {
                printf( "! SUT: Unable to open %s for reading\n", maillog );
                return false;
        }
        if( -1 == fstat( fd, &stat_buf ) ) {
                fprintf( stderr, "! sut: Unable to stat [%s]: [%s]\n",
                         maillog, strerror( errno ) );
                return false;
        }
        st_ino = stat_buf.st_ino;
        lseek( fd, 0, SEEK_END );
        rc = system( start );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                /* ask for options */
        }
        for( ;; ) {
                int i = 0;
                memset( buf, '\0', 512 );
                if( -1 == fstat( fd, &stat_buf ) ) {
                        fprintf( stderr, "! sut: Unable to stat [%s]: [%s]\n",
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
}

char **sut_listTests( const char *td, int *sz )
{
        DIR *dir;
        struct dirent *ent;
        int i = 0;

        char **buf = ( char ** )calloc( 8192, sizeof( char * ) );       //TODO
        if( !( dir = opendir( td ) ) ) {
                debug( "1: Can't open test directory [%s] : %s\n",
                       td, strerror( errno ) );
                exit( EXIT_FAILURE );
        }
        while( ( ent = readdir( dir ) ) ) {
                buf[i] = ( char * )calloc( 512, sizeof( char ) );
                strcpy( buf[i++], ent->d_name );
        }
        *sz = i;
        return buf;
}

/*
bool
sut_listOSes()
{
}*/

/*-------------------------------*/
/*     STOP   */
bool
sut_stop( int test_type,
          int timeout, char *maillog, char *stop, char *hostname, int port )
{
        printf( "* axiStop ...\n" );
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
}


/*
 * \todo Test for failure */
static bool
sut_stopRemote( int timeout,
                char *maillog, char *stop, char *hostname, int port )
{

        int sockfd = -1;
        char cmd[FILENAME_MAX] = { 0 };
        sockfd = sock_connectTo( hostname, port );
        sprintf( cmd, "STOP %d %s %s", timeout, maillog, stop );
        sock_sendLine( sockfd, cmd );
        close( sockfd );
        return true;
}


static bool sut_stopLocal( int timeout, char *maillog, char *stop )
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
}



/*  REFRESH */
bool
sut_refresh( int test_type, char *source, char *dest, char *host, int port )
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
}



static bool sut_refreshLocal( char *source, char *dest )
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
        if( dest[strlen( dest ) - 1] == '/' )
                dest[strlen( dest ) - 1] = '\0';
        sprintf( cmd, "/bin/cp -R %s %s", source, dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: File not copied\n" );
                return false;
        }
        return true;
}

static bool sut_refreshRemote( char *host, int port, char *sursa, char *dest )
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

        fori = 1;
        i <= nr_users;
        i++ delete_msgs(  ) {
        login select inbox store 1:*+flags \ deleted
                    a expunge} delete_folders(  ) {
                list "" * fori = nr_lines;
                i > 0;
        i-- delete mbox}
        end
#endif
            return true;
}


/*
 * Remote Refresh without restart/hup
 */
static bool sut_refreshRemoteWarm(  )
{
        return true;
}


bool
sut_checkCore( int test_type,
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
}


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
}


/*---------------*/
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

static bool sut_moveLog( const char *workDir, char *dst )
{
        char src[FILENAME_MAX] = { 0 };
        char cmd[2 * FILENAME_MAX + 32] = { 0 };
        sprintf( src, "%s/log/default.txt", workDir );
        sprintf( cmd, "/bin/mv %s %s", src, dst );
        system( cmd );
        return true;
}

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



void sut_sigpipe(  )
{
        printf( "Bailing out\n" );
        exit( EXIT_SUCCESS );
}

void sut_sigint(  )
{
        printf( "Bailing out\n" );
        exit( EXIT_SUCCESS );
}
