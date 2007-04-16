#include <limits.h>
#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"

/*
 * Server-side remote procedure call implemented over TCP.
 */

int t_startCallback( int sock, char *buf )
{
        int timeout = 0, rb;
        char *startCmd = NULL;
        char maillog[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! START: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        startCmd = strchr( buf + 1, ' ' );
        if( rb != 2 || !startCmd ) {
                printf( "Invalid syntax for START\n" );
                return FALSE;
        }

        printf( "START %d [%s][%s]\n", timeout, maillog, startCmd );
        sut_start( TEST_LOCAL, timeout, maillog, startCmd, 0, 0 );
        sock_sendStatus( sock, 0 );
        return TRUE;            /* @todo pls return a proper value(sendStatus) */
}


int t_stopCallback( int sock, char *buf )
{
        int timeout = 0, rb;
        char *stopCmd = NULL;
        char maillog[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! STOP: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        stopCmd = strchr( buf + 1, ' ' );
        if( rb != 2 || !stopCmd ) {
                printf( "Invalid syntax for STOP\n" );
                return FALSE;
        }
        printf( "STOP %d [%s][%s]\n", timeout, maillog, stopCmd );
        sut_stop( TEST_LOCAL, timeout, maillog, stopCmd, 0, 0 );
        sock_sendStatus( sock, 0 );
        return TRUE;
}

/**
 * \todo Replace this with a newer version 
 */
int t_refreshCallback( int sock, char *buf )
{
        char com[PATH_MAX] = { 0 };
        struct stat stats;
        char cale_s[PATH_MAX] = { 0 };
        char cale_d[PATH_MAX] = { 0 };
        int cod = 0, rb = 0;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! REFRESH: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s %s", cale_s, cale_d );

        cod = stat( cale_s, &stats );
        if( cod != 0 ) {
                printf( "rsh: ERR: Can't find %s\n", cale_s );
                exit( -1 );
        }
        sprintf( com, "rm -rf %s", cale_d );
        if( system( com ) == -1 ) {
                return errno;
        }
        sprintf( com, "/bin/cp -r %s %s", cale_s, cale_d );
        if( system( com ) == -1 ) {
                return errno;
        }

        sock_sendStatus( sock, 0 );
        return 0;
}



/**
 * \brief Read a stream for socket, and writes it to dest_dir/src_file.
 */
int t_copyCallback( int sock, char *buf )
{
        int f, bw, len, rb;     /* bytes written to disk */
        char buff[PATH_MAX] = { 0 };
        char dest_file[FILENAME_MAX] = { 0 };
        char src_file[FILENAME_MAX] = { 0 };
        char dest_dir[FILENAME_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! COPY: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s %s %d", src_file, dest_dir, &len );

        if( rb != 3 || len <= 0 ) {
                printf( "Invalid syntax for COPY\n" );
                return FALSE;
        }

        sprintf( dest_file, "%s/%s", dest_dir, src_file );
        f = creat( dest_file, S_IRUSR | S_IWUSR );
        if( f < 0 ) {
                sock_sendStatus( sock, errno );
                return errno;
        }
        while( len > 0 && ( bw = read( sock, buff, PATH_MAX - 1 ) ) ) {
                int ret = write( f, buff, bw < len ? bw : len );
                if( bw < 0 ) {
                        perror( "Transmission error" );
                        close( f );
                        return errno;
                }
                if( ret < 0 ) {
                        close( f );
                        return errno;
                }
                len -= bw;
        }

        sock_sendStatus( sock, 0 );
        close( f );
        return 0;
}



int t_rmCallback( int sock, char *buf )
{
        char cmd[PATH_MAX] = { 0 };
        int rb;
        char path[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! RM: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for RM\n" );
                return FALSE;
        }

        sprintf( cmd, "/bin/rm -rf %s", path );
        sock_sendStatus( sock, 0 );     /* fix this pls */
        return system( cmd );
}



int t_mkdirCallback( int sock, char *buf )
{
        int rb;
        char path[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! MKDIR: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for MKDIR\n" );
                return FALSE;
        }

        if( mkdir( path, 0777 ) == -1 ) {
                printf( "mkdir: ERR:[%s]", strerror( errno ) );
                sock_sendStatus( sock, errno );
                return errno;
        }
        sock_sendStatus( sock, 0 );
        return 0;
}

/**
 * @todo Look at strchr
 */
int t_executeCallback( int sock, char *buf )
{
        char *p = NULL;
        int rc = 0;

        p = buf;
        if( !p ) {
                printf( "ERR: null command\n" );
                return -1;
        }
        rc = system( p );
        sock_sendStatus( sock, 0 );
        return 0;
}



int t_checkCoreCallback( int sock, char *buf )
{
        char core_srcDir[FILENAME_MAX] = { 0 };
        char dbg_srcDir[FILENAME_MAX] = { 0 };
        char axi_workDir[FILENAME_MAX] = { 0 };
        char axi_cfgFile[FILENAME_MAX] = { 0 };
        char crash_destDir[FILENAME_MAX] = { 0 };
        int rc = FALSE;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! CHECKCORE (null) internal `buf` parameter\n" );
                return 0;
        }
        sscanf( buf, "%s %s %s %s %s",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        printf( "CORE %s %s %s %s %s",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        rc = sut_checkCore( TEST_LOCAL,
                            core_srcDir, dbg_srcDir, axi_workDir,
                            axi_cfgFile, crash_destDir );
        sock_sendStatus( sock, rc );    /*! @todo figure out what status i should send */
        return FALSE;           /* means no core was found */
}
