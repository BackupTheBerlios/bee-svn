#include "strop.h"
#include "socket.h"

/**
 * CALLBACK Functions, to deal with client requests. 
 */

int
callback_start( int sock, char *buf )
{
        int timeout = 0, rb;
        char *startCmd = NULL;
        char maillog[LINE_MAX] = { 0 } ;

        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        startCmd = strchr( buf+1, ' ' );
        if( rb != 2 || !startCmd ) {
                printf( "Invalid syntax for START\n" );
                return FALSE;
        }

        printf("START %d [%s][%s]\n", timeout, maillog, startCmd );
        sut_start( TEST_LOCAL, timeout, maillog, startCmd,0,0 );
        sock_sendStatus( sock, 0 );
        return TRUE;            /* @todo pls return a proper value(sendStatus) */
}


int
callback_stop( int sock, char *buf )
{
        int timeout = 0, rb;
        char *stopCmd = NULL;
        char maillog[LINE_MAX] = { 0 } ;

        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        stopCmd = strchr( buf+1, ' ' );
        if( rb != 2 || !stopCmd ) {
                printf( "Invalid syntax for STOP\n" );
                return FALSE;
        }
        printf("STOP %d [%s][%s]\n", timeout, maillog, stopCmd );
        sut_stop( TEST_LOCAL, timeout, maillog, stopCmd,0,0 );
        sock_sendStatus( sock, 0 );
        return TRUE;
}

/**
 * \todo Replace this with a newer version 
 */
int
callback_refresh( int sock, char *buf )
{
        char com[LINE_MAX] = { 0 };
        struct stat stats;
        char cale_s[LINE_MAX] = { 0 };
        char cale_d[LINE_MAX] = { 0 };
        int cod = 0, rb = 0;

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
int
callback_copy( int sock, char *buf )
{
        int f, bw, len, rb;     /* bytes written to disk */
        char buff[LINE_MAX] = { 0 };
        char dest_file[FILENAME_MAX] = { 0 };
        char src_file[FILENAME_MAX] = { 0 } ;
        char dest_dir[FILENAME_MAX] = { 0 } ;

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
        while( len > 0 && ( bw = read( sock, buff, LINE_MAX-1 ) ) ) {
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



int
callback_rm( int sock, char *buf )
{
        char cmd[LINE_MAX] = { 0 };
        int rb;
        char path[LINE_MAX] = { 0 };

        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for RM\n" );
                return FALSE;
        }

        sprintf( cmd, "/bin/rm -rf %s", path );
        sock_sendStatus( sock, 0 );     /* fix this pls*/
        return system( cmd );
}



int
callback_mkdir( int sock, char *buf )
{
        int rb;
        char path[LINE_MAX] = { 0 };

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
int
callback_execute( int sock, char *buf )
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



int
callback_checkCore( int sock, char *buf )
{
        char core_srcDir[FILENAME_MAX] = { 0 } ;
        char dbg_srcDir[FILENAME_MAX] = { 0 } ;
        char axi_workDir[FILENAME_MAX] = { 0 } ;
        char axi_cfgFile[FILENAME_MAX] = { 0 } ;
        char crash_destDir[FILENAME_MAX] = { 0 };
        int rc = FALSE;

        sscanf( buf, "%s %s %s %s %s",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        printf("CORE %s %s %s %s %s",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        rc = sut_checkCore( TEST_LOCAL,
                            core_srcDir, dbg_srcDir, axi_workDir,
                            axi_cfgFile, crash_destDir );
        sock_sendStatus( sock, rc );    /*! @todo figure out what status i should send*/
        return FALSE;           /* means no core was found */
}


