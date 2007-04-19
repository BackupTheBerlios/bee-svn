#include "xmlrpc.h"
#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"
#include "testdb.h"
#include <sys/wait.h>
#include <limits.h>


/*
 * Server-side remote procedure call implemented over XMLRPC.
 */


/* START timeout maillog startCommand */
    XMLRPC_VALUE
x_startCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    int timeout = 0, rc = 0;
    const char *startCmd, *maillog, *rs;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    timeout = XMLRPC_GetValueInt( xIter );

    xIter   = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    maillog = XMLRPC_GetValueString( xIter );

    xIter    = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    startCmd = XMLRPC_GetValueString( xIter );

    rc = sut_start( TEST_LOCAL, timeout, maillog, startCmd, NULL, 0 );
    rs = rc > 0 ? "Sut Started" : "Sut Failed to start";
    return XMLRPC_CreateValueString( NULL, rs, 0 );
}


    XMLRPC_VALUE
x_listTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    char** testList=NULL;
    int nbTests=0;
    XMLRPC_VALUE rv;

    testList = testdb_listTests( "/home/groleo/tests", &nbTests);
    rv = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
    while( nbTests-- ) {
        if(!XMLRPC_VectorAppendString( rv, NULL, testList[nbTests], 0 ))
        {    return NULL;}
        free(testList[nbTests]);
    }
    free(testList);
    return rv;
}


    XMLRPC_VALUE
x_listMachinesCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    char** testList=NULL;
    int nbMachines=0;
    XMLRPC_VALUE rv;

    testList = testdb_listMachines( MACHINES, &nbMachines);
    rv = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
    while( nbMachines-- ) {
        if(!XMLRPC_VectorAppendString( rv, NULL, testList[nbMachines], 0 ))
        {   return NULL; }
        free(testList[nbMachines]);
    }
    free(testList);
    return rv;
}


    XMLRPC_VALUE
x_getConfigCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    XMLRPC_VALUE rv;
    GSList* symbList=NULL;
    ConfigEntry* tmp=NULL;
    unsigned int nbSymbols=0;
    const char* machine=NULL;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    XMLRPC_VALUE ret = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
    rv  = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

    machine = XMLRPC_GetValueString( xIter );
    symbList = testdb_getConfig( machine , &nbSymbols);
    debug("UserData:%p\n", userData);
    while( nbSymbols-- ) {
        tmp = (ConfigEntry*)g_slist_nth_data(symbList, nbSymbols);
        if( (!tmp) || (!tmp->symbol) || (!tmp->value) ) continue;
        rv = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);
        XMLRPC_VectorAppendString( rv, "symbol", tmp->symbol, 0 );
        XMLRPC_VectorAppendString( rv, "val" , tmp->value, 0 );
        XMLRPC_AddValueToVector ( ret, rv);
        //free(symbList[nbSymbols].symbol);
        //free(symbList[nbSymbols].value);
    }
    //free(symbList);
    return ret;
}


    XMLRPC_VALUE
x_setConfigCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    XMLRPC_VALUE rv;
    char  path[PATH_MAX]={0};
    const char *p=NULL, *machine=NULL;
    XMLRPC_VALUE cfg_lines,str;
    FILE* f;

    const XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    rv  = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

    str = XMLRPC_VectorRewind(XMLRPC_RequestGetData(request));

    /* Extract machine Name */
    machine = XMLRPC_VectorGetStringWithID(str, "sut_machine");
    debug("SUT machine:%s\n", machine);

    sprintf( path, "%s/%s", MACHINES, machine );
    f = fopen( path, "w");
    if(!f) {
        debug("Unable to open [%s]\n", path);
        exit(EXIT_FAILURE);
    }else
        debug("Opened [%s]\n", path);

    /* Extract Config Lines */
    cfg_lines = XMLRPC_VectorGetValueWithID(str, "cfg_lines");
    xIter = XMLRPC_VectorRewind( cfg_lines );
    while(xIter) {
        p = XMLRPC_GetValueString( xIter );
        debug("LINE: [%s]\n", p);
        xIter = XMLRPC_VectorNext(cfg_lines);
        if( EOF == fprintf( f, "%s", p) ) {
            debug("Cant write to config\n");
            exit(EXIT_FAILURE);
        }
    }
    //free(symbList);
    fflush(f);
    fclose(f);
    return rv;
}



    XMLRPC_VALUE
x_stopCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    int timeout = 0, rc = 0;
    const char *stopCmd;
    const char *maillog;
    const char *rs;

    const XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );
    timeout = XMLRPC_GetValueInt( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    maillog = XMLRPC_GetValueString( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    stopCmd = XMLRPC_GetValueString( xIter );

    rc = sut_stop( TEST_LOCAL, timeout, maillog, stopCmd, NULL, 0 );
    rs = rc > 0 ? "Sut Stopped" : "Sut Failed to stop";
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
    //        int rc = 0;

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
x_addMachineCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    const char *name, *OS, *OSVer, *IP;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

    name  = XMLRPC_VectorGetStringWithID(xIter, "sut_mname");
    OS    = XMLRPC_VectorGetStringWithID(xIter, "sut_os");
    OSVer = XMLRPC_VectorGetStringWithID(xIter, "sut_osver");
    IP    = XMLRPC_VectorGetStringWithID(xIter, "sut_mip");

    testdb_addMachine( name, OS, OSVer, IP );

    return XMLRPC_CreateValueString( NULL, "Machine added", 0 );
}


    XMLRPC_VALUE
x_runTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData )
{
    XMLRPC_VALUE str;
    const char* sut_build, *p, *os;
    XMLRPC_VALUE oses, tests;
    XMLRPC_VALUE xIter ;
    XMLRPC_VALUE xStarted;
    extern struct config_s cfg;
    pid_t pid;
    int status=0;


    str = XMLRPC_VectorRewind(XMLRPC_RequestGetData(request));

    /* Extract SUT Build */
    sut_build = XMLRPC_VectorGetStringWithID(str, "sut_build");
    debug("SUT Build:%s\n", sut_build);

    /* Extract OSes */
    oses = XMLRPC_VectorGetValueWithID(str, "sut_os");
    xIter = XMLRPC_VectorRewind( oses );
    while(xIter) {
        os = XMLRPC_GetValueString( xIter );
        debug("OSes: %s\n", os);
        xIter = XMLRPC_VectorNext(oses);
    }

    /* Extract and RUN SUT Tests */
    tests = XMLRPC_VectorGetValueWithID(str, "sut_tests");
    xIter = XMLRPC_VectorRewind( tests );

    pid = fork() ;

    if(pid<0) {
        debug("ERR:cant start the tests\n");
        return XMLRPC_CreateValueString( NULL, "Unable to start the tests", 0 );
    }
    if(pid>0) {
        debug("father returned\n");
        //waitpid(pid, &status, WNOHANG);
        return XMLRPC_CreateValueString( NULL, "Started Test Execution", 0 );
    }
    if(!pid) {
        while(xIter) {
            //TODO: lots of BOF
            char tDir[PATH_MAX] = {0};
            char tCfg[PATH_MAX] = {0};
            char cmd[PATH_MAX*3]={0};

            p = XMLRPC_GetValueString( xIter );
            debug("Run tests from directory: '%s/%s' \n", cfg.test_dir,p);
            sprintf(tDir, "%s/%s", cfg.test_dir,p);
            sprintf(tCfg, "%s/%s", MACHINES,os);
            sprintf(cmd,"/home/groleo/code/seraph/src/bin/seraph -C %s -d %s -t local -r n -k"
            ,tCfg, tDir
            );
            system(cmd);
            xIter = XMLRPC_VectorNext(tests);
        }
        sleep(20);
        exit(EXIT_SUCCESS);
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
    rc = core_checkCore( TEST_LOCAL,
            core_srcDir, dbg_srcDir, axi_workDir,
            axi_cfgFile, crash_destDir );

    return XMLRPC_CreateValueString( NULL, "No core found", 0 );
}



    XMLRPC_VALUE
x_registerUserCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    testdb_register( name, email, username, password)
}

    XMLRPC_VALUE
x_checkSessionCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{

    testdb_checkSession( username, cookie, session, ip );

}


    XMLRPC_VALUE
x_setSessionCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    testdb_setSession( id, session, ip);
}


    XMLRPC_VALUE
x_checkLoginCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    testdb_checkLogin( username, password);
}


    XMLRPC_VALUE
x_checkRemembered( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    testdb_checkRemembered( username, cookie );
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
