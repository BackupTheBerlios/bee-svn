#include "config.h"
#include "dbg.h"
#include "strop.h"
#include "sock.h"
#include "sut.h"
#include "core.h"

extern struct config_s cfg;
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
                sock_sendStatus( sock, 1 );
                return false;
        }
        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        startCmd = strchr( buf + 1, ' ' );
        if( rb != 2 || !startCmd ) {
                printf( "Invalid syntax for START\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }

        dbg_verbose( "START timeout[%d] maillog[%s] startCmd[%s]\n", timeout, maillog, startCmd );
        sut_start( TEST_LOCAL, timeout, maillog, startCmd, 0, 0 );
        sock_sendStatus( sock, 0 );
        return true;            /* @todo pls return a proper value(sendStatus) */
}


int t_stopCallback( int sock, char *buf )
{
        int timeout = 0, rb;
        char *stopCmd = NULL;
        char maillog[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! STOP: (null) internal `buf` parameter\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }
        rb = sscanf( buf, "%d %s", &timeout, maillog );

        buf = strchr( buf, ' ' );
        stopCmd = strchr( buf + 1, ' ' );
        if( rb != 2 || !stopCmd ) {
                printf( "Invalid syntax for STOP\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }
        dbg_verbose( "STOP timeout[%d] maillog[%s] stopCmd[%s]\n", timeout, maillog, stopCmd );
        sut_stop( TEST_LOCAL, timeout, maillog, stopCmd, 0, 0 );
        sock_sendStatus( sock, 0 );
        return true;
}

/**
 * \todo Replace this with a newer version 
 */
int t_refreshCallback( int sock, char *buf )
{
        int ret = 0;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! REFRESH: (null) internal `buf` parameter\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }
        ret = system(buf); /*TODO: check return values*/

        sock_sendStatus( sock, 0 );
        return true;
}



/**
 * \brief Read a stream for socket, and writes it to dest_dir/src_file.
 */
int t_copyCallback( int sock, char *buf )
{
        int f, bw, len, rb;     /* bytes written to disk */
        char buff[PATH_MAX] = { 0 };
        char src_file[FILENAME_MAX] = { 0 };
        char dest_dir[FILENAME_MAX] = { 0 };
        char *dest_file=NULL;
        struct stat stbuf;

        if( NULL == buf ) {
                fprintf( stderr, "! COPY: (null) internal `buf` parameter\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }
        rb = sscanf( buf, "%s %s %d", src_file, dest_dir, &len );

        if( rb != 3 || len <= 0 ) {
                printf( "Invalid syntax for COPY\n" );
                return false;
        }

        if( lstat( dest_dir, &stbuf) )
        {   sock_sendStatus( sock, errno) ;
            return errno;
        }

        if( S_ISDIR(stbuf.st_mode) )
        {   dest_file=(char*)malloc(strlen(dest_dir)+strlen(src_file)+2);
            sprintf( dest_file, "%s/%s", dest_dir, src_file );
        }
        else if( S_ISREG(stbuf.st_mode) )
            dest_file = strdup(dest_dir) ;

        f = creat( dest_file, S_IRUSR | S_IWUSR );
        if( f < 0 )
        {       free(dest_file);
                sock_sendStatus( sock, errno );
                return errno;
        }
        while( len > 0 && ( bw = read( sock, buff, PATH_MAX - 1 ) ) )
        {       int ret = write( f, buff, bw < len ? bw : len );
                debug("len(%d) bw(%d)\n", len, bw);
                if( bw < 0 ) {
                        perror( "Transmission error" );
                        close( f );
                        free(dest_file);
                        sock_sendStatus( sock, errno );
                        return errno;
                }
                if( ret < 0 )
                {   close( f );
                    free(dest_file);
                    sock_sendStatus( sock, errno );
                    return errno;
                }
                len -= bw;
        }

        sock_sendStatus( sock, 0 );
        close( f );
        free(dest_file);
        return 0;
}



int t_rmCallback( int sock, char *buf )
{
        char cmd[PATH_MAX] = { 0 };
        int rb;
        char path[PATH_MAX] = { 0 };

        if( NULL == buf ) {
                fprintf( stderr, "! RM: (null) internal `buf` parameter\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for RM\n" );
                return false;
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
                sock_sendStatus( sock, 1 );
                return false;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for MKDIR\n" );
                sock_sendStatus( sock, 1 );
                return false;
        }

        if( mkdir( path, 0777 ) == -1 ) {
                dbg_verbose( "mkdir: ERR:[%s]\n", strerror( errno ) );
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
        char coreSrcDir[FILENAME_MAX] = { 0 };
        char dbgSrcDir[FILENAME_MAX] = { 0 };
        char sutWorkDir[FILENAME_MAX] = { 0 };
        char sutCfgFile[FILENAME_MAX] = { 0 };
        char dumpDestDir[FILENAME_MAX] = { 0 };
        char curTest[FILENAME_MAX] = { 0 };
        int rc = false;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! CHECKCORE (null) internal `buf` parameter\n" );
                return 0;
        }
        sscanf( buf, "%s %s %s %s %s %s",
                coreSrcDir, dbgSrcDir, sutWorkDir,
                sutCfgFile, dumpDestDir, curTest );
        dbg_verbose( "CORE core_srcDir[%s] dbg_srcDir[%s] sutWorkDir[%s] sutCfgFile[%s] crash_destDir[%s] curTest[%s]\n",
                coreSrcDir, dbgSrcDir, sutWorkDir,
                sutCfgFile, dumpDestDir, curTest );
        rc = core_checkCore( TEST_LOCAL,
                            coreSrcDir, dbgSrcDir, sutWorkDir,
                            sutCfgFile, dumpDestDir, curTest );
        sock_sendStatus( sock, rc );
        return false;           /* means no core was found */
}
