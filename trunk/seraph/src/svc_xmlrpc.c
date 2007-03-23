#include "xmlrpc.h"
#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"

/*
 * Server-side remote procedure call implemented over XMLRPC.
 */


/* START timeout maillog startCommand */
XMLRPC_VALUE
x_startCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
        int timeout = 0, rc = 0;
        const char *startCmd;
        const char *maillog;
        const char *rs;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );
        timeout = XMLRPC_GetValueInt( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        maillog = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        startCmd = XMLRPC_GetValueString( xIter );

        printf( "START [%d][%s][%s]\n", timeout, maillog, startCmd );
        rc = sut_start( TEST_LOCAL, timeout, maillog, startCmd, 0, 0 );
        rs = rc > 0 ? "Started" : "Failed to start";
        return XMLRPC_CreateValueString( NULL, rs, 0 );
}

XMLRPC_VALUE
x_listTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
                     void *userData )
{
        const char *rs;
        char **testList = 0;
        int nbTests = 0;
        XMLRPC_VALUE rv;

        printf( "LISTTESTS\n" );

        rv = XMLRPC_CreateVector( NULL, xmlrpc_vector_array );
        testList = sut_listTests( "/home/groleo/tests", &nbTests );
        while( nbTests-- ) {
                XMLRPC_VectorAppendString( rv, NULL, testList[nbTests], 0 );
                free( testList[nbTests] );
        }
        free( testList );
        return rv;
}


XMLRPC_VALUE
x_stopCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
        int timeout = 0, rc = 0;
        const char *stopCmd;
        const char *maillog;
        const char *rs;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );
        timeout = XMLRPC_GetValueInt( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        maillog = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        stopCmd = XMLRPC_GetValueString( xIter );

        printf( "STOP [%d][%s][%s]\n", timeout, maillog, stopCmd );
        rc = sut_stop( TEST_LOCAL, timeout, maillog, stopCmd, 0, 0 );
        rs = rc > 0 ? "Stopped" : "Failed to stop";
        return XMLRPC_CreateValueString( NULL, rs, 0 );
}

XMLRPC_VALUE
x_rmCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
        char cmd[PATH_MAX] = { 0 };
        const char *path, *rs;
        int rc = 0;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

        path = XMLRPC_GetValueString( xIter );

        sprintf( cmd, "/bin/rm -rf %s", path );
        rc = system( cmd );
        rs = rc >
            0 ? "Remove operation successfull" : "Remove operation failed";
        return XMLRPC_CreateValueString( NULL, rs, 0 );
}



XMLRPC_VALUE
x_mkdirCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
        const char *path;
        int rc = 0;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

        path = XMLRPC_GetValueString( xIter );

        if( mkdir( path, 0777 ) == -1 ) {
                printf( "mkdir: ERR:[%s]", strerror( errno ) );
                return XMLRPC_CreateValueString( NULL, "mkdir FAILED", 0 );
        }
        return XMLRPC_CreateValueString( NULL, "mkdir OK", 0 );
}

/**
 * @todo Look at strchr
 */
XMLRPC_VALUE
x_executeCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
                   void *userData )
{
        const char *p;
        int rc = 0;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

        p = XMLRPC_GetValueString( xIter );
        rc = system( p );
        return XMLRPC_CreateValueString( NULL, "EXECUTE ok", 0 );
}


XMLRPC_VALUE
x_runTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
                    void *userData )
{
        XMLRPC_VALUE str;
        char *sut_build, *p;
        XMLRPC_VALUE oses, tests;
        XMLRPC_VALUE xIter;
        extern struct config_s cfg;
        pid_t pid;


        str = XMLRPC_VectorRewind( XMLRPC_RequestGetData( request ) );

        /* Extract SUT Build */
        sut_build = XMLRPC_VectorGetStringWithID( str, "sut_build" );
        debug( "SUT Build:%s\n", sut_build );

        /* Extract OSes */
        oses = XMLRPC_VectorGetValueWithID( str, "sut_os" );
        xIter = XMLRPC_VectorRewind( oses );
        while( xIter ) {
                p = XMLRPC_GetValueString( xIter );
                debug( "OSes: %s\n", p );
                xIter = XMLRPC_VectorNext( oses );
        }

        /* Extract and RUN SUT Tests */
        tests = XMLRPC_VectorGetValueWithID( str, "sut_tests" );
        xIter = XMLRPC_VectorRewind( tests );
        pid = fork(  );

        if( pid < 0 ) {
                debug( "ERR:cant start the tests\n" );
                return -1;
        }
        if( pid > 0 ) {
                int status;
                wait( &status );
                return XMLRPC_CreateValueString( NULL,
                                                 "Starting Test Execution", 0 );
        }
        if( !pid ) {
                while( xIter ) {
                        char b[PATH_MAX] = { 0 };
                        p = XMLRPC_GetValueString( xIter );
                        debug( "Run tests from directory: '%s/%s' \n",
                               cfg.test_dir, p );
                        sprintf( b, "%s/%s", cfg.test_dir, p );
                        srph_runTests( b );
                        xIter = XMLRPC_VectorNext( tests );
                }
        }
}

/* chechcore( core_srcDir, dbg_srcDir, axi_workDir, axi_cfgFile, crash_destDir ) */
XMLRPC_VALUE
x_checkCoreCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
                     void *userData )
{
        const char *core_srcDir;
        const char *dbg_srcDir;
        const char *axi_workDir;
        const char *axi_cfgFile;
        const char *crash_destDir;
        int rc = FALSE;

        XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
        XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );
        core_srcDir = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        dbg_srcDir = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        axi_workDir = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        axi_cfgFile = XMLRPC_GetValueString( xIter );

        xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
        crash_destDir = XMLRPC_GetValueString( xIter );

        printf( "CORE %s %s %s %s %s\n",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        rc = sut_checkCore( TEST_LOCAL,
                            core_srcDir, dbg_srcDir, axi_workDir,
                            axi_cfgFile, crash_destDir );

        return XMLRPC_CreateValueString( NULL, "No core found", 0 );
}

#if 0
/**
 * \todo Replace this with a newer version 
 */
int x_refreshCallback( int sock, char *buf )
{
        char com[LINE_MAX] = { 0 };
        struct stat stats;
        char cale_s[LINE_MAX] = { 0 };
        char cale_d[LINE_MAX] = { 0 };
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
int x_copyCallback( int sock, char *buf )
{
        int f, bw, len, rb;     /* bytes written to disk */
        char buff[LINE_MAX] = { 0 };
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
        while( len > 0 && ( bw = read( sock, buff, LINE_MAX - 1 ) ) ) {
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


#endif
