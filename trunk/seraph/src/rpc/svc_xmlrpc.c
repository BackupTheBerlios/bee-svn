#include "xmlrpc.h"
#include "config.h"
#include "dbg.h"
#include "strop.h"
#include "sock.h"
#include "sut.h"
#include "core.h"
#include "new.h"
#include "testdb.h"
#include "userdb.h"
#include "basedb.h"
#include <sys/wait.h>
#include <limits.h>


/*
 * Server-side remote procedure call implemented over XMLRPC.
 */

/* START timeout maillog startCommand
 * machine, user*/
    XMLRPC_VALUE
x_startManagerCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    int timeout = 0, rc = 0;
    const char *uName, *machine, *rs;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );

    uName    = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    machine  = XMLRPC_VectorGetStringWithID(xIter,"sut_machine");

    rc = sut_startManager( uName, machine);
    rs = rc > 0 ? "Manager Started" : "Manager Failed to start";
    return XMLRPC_CreateValueString( NULL, rs, 0 );
}

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
    extern struct config_s cfg;
    char** testList=NULL;
    int nbTests=0;
    XMLRPC_VALUE rv;

    testList = testdb_listTests( cfg.testDir, &nbTests);
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
x_listUserTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    extern struct config_s cfg;
    char** testList=NULL;
    int nbTests=0;
    XMLRPC_VALUE rv;
    const char* uName;

    XMLRPC_VALUE xParams= XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter  = XMLRPC_VectorRewind( xParams );
    uName    = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    testList = testdb_listUserTests( uName, &nbTests);
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
x_clearRunnedJobsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    int nbTests=0;
    XMLRPC_VALUE rv;
    bool rc=false;
    const char* uName, *rs;

    XMLRPC_VALUE xParams= XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter  = XMLRPC_VectorRewind( xParams );
    uName    = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    rc = testdb_clearRunnedJobs(uName);
    rs = rc ? "Cleared" : "Unable to clear";
    return XMLRPC_CreateValueString( NULL, rs, 0 );
}

    XMLRPC_VALUE
x_listMachinesCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    char** testList=NULL;
    int nbMachines=0;
    XMLRPC_VALUE rv;
    const char *uName;

    XMLRPC_VALUE xParams= XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter  = XMLRPC_VectorRewind( xParams );
    uName  = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    testList = testdb_listMachines( uName, &nbMachines);
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
    GSList* symbList=NULL;
    ConfigEntry* tmp=NULL;
    unsigned int nbSymbols=0;
    const char* machine=NULL, *uName=NULL;

    XMLRPC_VALUE xParams= XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter  = XMLRPC_VectorRewind( xParams );
    XMLRPC_VALUE ret    = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
    XMLRPC_VALUE tmpv ;

    uName    = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    machine  = XMLRPC_VectorGetStringWithID(xIter,"sut_machine");
    symbList = testdb_getConfig( uName, machine , &nbSymbols);
    while( nbSymbols-- )
    {
        tmp = (ConfigEntry*)g_slist_nth_data(symbList, nbSymbols);
        if( (!tmp) || (!tmp->symbol) || (!tmp->value) ) continue;

        tmpv = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);
        XMLRPC_VectorAppendString( tmpv, "symbol", tmp->symbol, 0 );
        XMLRPC_VectorAppendString( tmpv, "val" , tmp->value, 0 );
        XMLRPC_AddValueToVector ( ret, XMLRPC_CopyValue(tmpv));

        XMLRPC_CleanupValue(tmpv);
        free(tmp->symbol);
        free(tmp->value);
        free(tmp), tmp=NULL;
    }
    g_slist_free(symbList);
    return ret;
}

    XMLRPC_VALUE
x_listJobsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    GSList*   jobList =NULL ;
    int nbJobs=0, job_type=0;
    XMLRPC_VALUE ret;
    const char *username;
    struct job* tmp;
    XMLRPC_VALUE tmpv;
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );

    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    job_type  = XMLRPC_VectorGetIntWithID(xIter, "job_type");

    nbJobs = userdb_listJobs( username, job_type, &jobList);
    ret = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
    debug("VectorAppendString [%d] times\n", nbJobs);
    nbJobs++; /*g_slist_nth_data is one-based*/
    while( nbJobs-- ) {
        tmp = (struct job*)g_slist_nth_data( jobList, nbJobs);

        if(!tmp) { debug("continue\n"); continue;}
        debug("Vector<<Job [%d]\n", nbJobs);
        tmpv = XMLRPC_CreateVector( NULL, xmlrpc_vector_struct);
        XMLRPC_VectorAppendString( tmpv, "job_name", tmp->name, 0 );
        XMLRPC_VectorAppendString( tmpv, "job_date", tmp->date, 0 );
        XMLRPC_VectorAppendString( tmpv, "job_time", tmp->time, 0 );
        XMLRPC_VectorAppendString( tmpv, "job_tests", tmp->tests, 0 );
        XMLRPC_VectorAppendString( tmpv, "job_ctest", tmp->ctest, 0 );
        XMLRPC_AddValueToVector( ret, XMLRPC_CopyValue(tmpv) );

        XMLRPC_CleanupValue(tmpv);
        free(tmp->name);
        free(tmp->date);
        free(tmp->time);
        free(tmp->tests);
        free(tmp->ctest);
        free(tmp), tmp=NULL;
    }
    g_slist_free(jobList);
    return ret;
}


    XMLRPC_VALUE
x_setConfigCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void *userData )
{
    XMLRPC_VALUE rv;
    char  path[PATH_MAX]={0};
    const char *p=NULL, *machine=NULL, *uName=NULL;
    XMLRPC_VALUE cfg_lines,str;
    FILE* f;
    int ret=0;

    const XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    rv  = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

    str = XMLRPC_VectorRewind(XMLRPC_RequestGetData(request));

    /* Extract machine Name */
    uName   = XMLRPC_VectorGetStringWithID(str, "sut_username");
    machine = XMLRPC_VectorGetStringWithID(str, "sut_machine");
    debug("SUT machine:%s\n", machine);

    ret = snprintf( path, PATH_MAX, "%s/%s/machines/%s", USERDB, uName, machine );
    if(ret >= PATH_MAX)
            path[PATH_MAX-1]='\0';

    f = fopen( path, "w");
    if(!f) {
        dbg_error("Unable to open [%s]: [%s]\n", path, strerror(errno));
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
            dbg_error("Cant write to config [%s]: [%s]\n",
                path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    debug("Wrote ok\n");
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
    /*
    char cmd[PATH_MAX] = { 0 };
    const char *path, *rs;
    int rc = 0;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

    path = XMLRPC_GetValueString( xIter );

    sprintf( cmd, "/bin/rm -rf %s", path );
    rc = system( cmd );
    */
    return XMLRPC_CreateValueBoolean( NULL, false );
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
        return XMLRPC_CreateValueBoolean( NULL, false );
    }
    return XMLRPC_CreateValueBoolean( NULL, true );
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
    const char *mUser, *mName, *mOS, *mOSVer, *mIP;
    bool ret=false;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

    mUser  = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    mName  = XMLRPC_VectorGetStringWithID(xIter, "sut_mname");
    mOS    = XMLRPC_VectorGetStringWithID(xIter, "sut_os");
    mOSVer = XMLRPC_VectorGetStringWithID(xIter, "sut_osver");
    mIP    = XMLRPC_VectorGetStringWithID(xIter, "sut_mip");

    ret = testdb_addMachine( mUser, mName, mOS, mOSVer, mIP );

    return XMLRPC_CreateValueBoolean( NULL, ret);
}

#warning COPY PASTE
    XMLRPC_VALUE
x_addUserTest( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    const char *mUser, *fData;
    int fSize=0;
    bool ret=false;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );

    mUser  = XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    fSize  = XMLRPC_VectorGetIntWithID(xIter, "file_size");
    fData  = XMLRPC_VectorGetBase64WithID(xIter, "uploadedfile");

    ret = testdb_addUserTest( mUser, fSize, fData );

    return XMLRPC_CreateValueBoolean( NULL, ret);
}

    XMLRPC_VALUE
x_runTestsCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData )
{
    XMLRPC_VALUE str;
    const char *sut_build=NULL, *p=NULL, *os=NULL, *uName=NULL, email[KEY_LEN+1]={0};
    XMLRPC_VALUE oses, tests;
    XMLRPC_VALUE xIter ;
    XMLRPC_VALUE xStarted;
    extern struct config_s cfg;
    pid_t pid;
    int status=0;
    const char* sut_refresh=NULL;


    str = XMLRPC_VectorRewind(XMLRPC_RequestGetData(request));

    sut_refresh = XMLRPC_VectorGetStringWithID(str, "sut_refresh");

    uName = XMLRPC_VectorGetStringWithID(str, "sut_username");

    /* Get the email of the user */
    struct BaseDB* db = new(BaseDB);
    db_open( db, "%s/%s/%s", USERDB, uName, "userdata");
    if( !db_get( db, "email", &email)
    ||  !db_close(db)
    )
        return false;
    delete(db);

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

    signal(SIGCHLD, core_onSigChld);

    pid = fork() ;

    if(pid<0) {
        debug("ERR:cant start the tests\n");
        return XMLRPC_CreateValueString( NULL, "Unable to start the tests", 0 );
    }
    if(pid>0) {
        printf("Father returned\n");
        int*p=malloc(sizeof(int));
        *p=-pid;
        *cfg.children = g_slist_append(*(cfg.children), p);

        return XMLRPC_CreateValueString( NULL, "Started Test Execution", 0 );
    }
    if(!pid)
    {   /* Move child in it's own group */
        printf("Setting sid\n");

        if( setpgid( 0, 0) )
        {
            printf( "Muttley you snickering hound....\n");
            exit(EXIT_FAILURE);
        }
        /*core_setSigHandlers();*/
        while(xIter)
        {   //TODO: use malloc
            char tDir[PATH_MAX] = {0};
            char tCfg[PATH_MAX] = {0};
            char cmd[PATH_MAX*3]={0};
            p = XMLRPC_GetValueString( xIter );
            /*TODO: use snprintf */
            /*TODO: don't use acces twice: add another param in xmlrpc struct*/
            sprintf(tDir, "%s/%s", cfg.testDir,p);
            if( access(tDir, X_OK))
            {   sprintf(tDir, "%s/%s/tests/%s", USERDB, uName, p);
                if(access(tDir,X_OK)) continue;
            }
            dbg_verbose("Run tests from directory: '%s/%s' \n", cfg.testDir,p);

            sprintf(tCfg, "%s/%s/machines/%s", USERDB, uName, os);
            sprintf(cmd,"%s/srph -M %s:axigen.gecadco.local -v -C %s -d %s -t remote -r %s -k -u %s",
                   BINDIR, email, tCfg, tDir, sut_refresh, uName);
            dbg_verbose("Seraph: [%s]\n", cmd);
            system(cmd);
            xIter = XMLRPC_VectorNext(tests);
        }
        exit(EXIT_SUCCESS);
    }
    return 0; //never reached
}

/* chechcore( coreSrcDir, dbgSrcDir, sutWorkDir, sutCfgFile, dumpDestDir ) */
    XMLRPC_VALUE
x_checkCoreCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    const char *coreSrcDir;
    const char *dbgSrcDir;
    const char *sutWorkDir;
    const char *sutCfgFile;
    const char *crash_destDir;
    int rc = FALSE;

    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter = XMLRPC_VectorRewind( xParams );
    coreSrcDir = XMLRPC_GetValueString( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    dbgSrcDir = XMLRPC_GetValueString( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    sutWorkDir = XMLRPC_GetValueString( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    sutCfgFile = XMLRPC_GetValueString( xIter );

    xIter = XMLRPC_VectorNext( XMLRPC_RequestGetData( request ) );
    crash_destDir = XMLRPC_GetValueString( xIter );

    printf( "CORE %s %s %s %s %s\n",
            coreSrcDir, dbgSrcDir, sutWorkDir,
            sutCfgFile, crash_destDir );
    /*rc = core_checkCore( TEST_LOCAL,
            coreSrcDir, dbgSrcDir, sutWorkDir,
            sutCfgFile, crash_destDir );
    */
    return XMLRPC_CreateValueBoolean( NULL, rc );
}



    XMLRPC_VALUE
x_registerUserCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    bool ret=false;
    //deserialize
    const char *name, *email, *username, *password;
    name    = XMLRPC_VectorGetStringWithID(xIter, "sut_name");
    email   = XMLRPC_VectorGetStringWithID(xIter, "sut_email");
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    password= XMLRPC_VectorGetStringWithID(xIter, "sut_password");
    ret = userdb_register( name, email, username, password);

    return XMLRPC_CreateValueBoolean(NULL, ret);
}

    XMLRPC_VALUE
x_checkSessionCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    bool ret=false;
    //deserialize
    const char *username, *cookie, *session, *ip;
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    cookie  = XMLRPC_VectorGetStringWithID(xIter, "sut_cookie");
    session = XMLRPC_VectorGetStringWithID(xIter, "sut_session");
    ip      = XMLRPC_VectorGetStringWithID(xIter, "sut_ip");
    debug("user[%s]\n", username);
    ret = userdb_checkSession( username, cookie, session, ip );

    return XMLRPC_CreateValueBoolean(NULL, ret );
}


    XMLRPC_VALUE
x_setSessionCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    bool ret=false;
    //deserialize
    const char *session, *ip, *username;
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    session = XMLRPC_VectorGetStringWithID(xIter, "sut_session");
    ip      = XMLRPC_VectorGetStringWithID(xIter, "sut_ip");
    ret = userdb_setSession( username, session, ip);
    return XMLRPC_CreateValueBoolean(NULL, ret);
}


    XMLRPC_VALUE
x_checkLoginCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    bool ret=false;
    //deserialize
    const char *username, *password;
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    password= XMLRPC_VectorGetStringWithID(xIter, "sut_password");
    ret = userdb_checkLogin( username, password);
    return XMLRPC_CreateValueBoolean(NULL, ret);
}


    XMLRPC_VALUE
x_checkRememberedCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    bool ret=false;
    //deserialize
    const char *username, *cookie;
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    cookie  = XMLRPC_VectorGetStringWithID(xIter, "sut_cookie");
    ret = userdb_checkRemembered( username, cookie );
    return XMLRPC_CreateValueBoolean(NULL, ret);
}


XMLRPC_VALUE
x_getErrorLogCallback( XMLRPC_SERVER server, XMLRPC_REQUEST request,
        void *userData )
{
    char    *b64;
    const char*log, *username;
    size_t  b64Len=0;
    XMLRPC_VALUE xParams = XMLRPC_RequestGetData( request );
    XMLRPC_VALUE xIter   = XMLRPC_VectorRewind( xParams );
    XMLRPC_VALUE ret;
    //deserialize
    username= XMLRPC_VectorGetStringWithID(xIter, "sut_username");
    log     = XMLRPC_VectorGetStringWithID(xIter, "sut_errlog");
    b64Len = userdb_getErrorLog( username, 2, log, &b64);
    ret = XMLRPC_CreateValueBase64( NULL, b64, b64Len);
    free(b64);
    return ret;
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
