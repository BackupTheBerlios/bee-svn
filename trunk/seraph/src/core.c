#include "config.h"
#include "dbg.h"
#include "sut.h"
#include "sock.h"
#include "strop.h"
#include "fileop.h"
#include "basedb.h"
#include "core.h"

extern struct config_s cfg;
static bool
core_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
                      const char *axi_workDir, const char *axi_cfgFile,
                      const char *crash_destDir );
static bool
core_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                     const char *workDir, const char *cfgFile,
                     const char *crash_destDir );

static int core_runBat( const char *bat_name, int timeout );

static int core_setupTmp( char const *source_bat, char *tmpDir );

static int core_parseBat( const char *filename );

static int core_dirAction( const char *fileName, struct stat *statbuf,
                           void *junk );

static int core_fileAction( const char *fileName, struct stat *statbuf,
                            void *junk );

static int core_cleanupTmp( const char *tmpDir );

static int core_runRecursive( const char *srcName );




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sig_handler( int sig )
{
    int status;
    switch ( sig ) {
        case SIGCHLD:
            while( waitpid( -1, &status, WNOHANG ) > 0 )
                if(  WIFEXITED(status) && (WEXITSTATUS( status ) == 69) )
                    printf( "* seraph: PASS\n" );
                else
                    dbg_error( "seraph: FAIL [%d]\n", WEXITSTATUS(status) );
            break;
        case SIGALRM:
            dbg_error( "timeout\n" );
            break;
    }
}





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bool
core_checkCore( const int test_type,
        const char *core_srcDir, const char *dbg_srcDir,
        const char *axi_workDir, const char *axi_cfgFile,
        const char *crash_destDir )
{
    if( test_type == TEST_LOCAL )
        return core_checkCoreLocal( core_srcDir, dbg_srcDir,
                axi_workDir, axi_cfgFile,
                crash_destDir );

    if( test_type == TEST_REMOTE )
        return core_checkCoreRemote( core_srcDir, dbg_srcDir,
                axi_workDir, axi_cfgFile,
                crash_destDir );
    return false;
}/*------------------------------------------------------------------*/



    static bool
core_checkCoreRemote( const char *core_srcDir, const char *dbg_srcDir,
        const char *axi_workDir, const char *axi_cfgFile,
        const char *crash_destDir )
{
    char cmd[PATH_MAX] = { 0 };     /* Max line should therefore be 3 times the length of FILENAME_MAX */
    int ret = 0;
    int sock;

    sock = sock_connectTo( cfg.hostname, cfg.rawport );
    if(sock==-1)
        return false;
    sprintf( cmd, "CHECKCORE %s %s %s %s %s",
            core_srcDir, dbg_srcDir, axi_workDir,
            axi_cfgFile, crash_destDir );
    ret = sock_sendLine( sock, cmd ); if( !ret ) return false ;
    ret = sock_getStatus( sock );
    close( sock );
    if( !ret ) return false;
    return true;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static bool
core_setupDstDir( char *dst, char *coreName,
        const char *core_srcDir, const char *core_dstDir )
{
    sprintf( dst, "%s/dumps/%s-%s", core_dstDir, cfg.cur_test, coreName );
    if( mkdir( dst, 0777 ) == -1 ) {
        dbg_error("srph: Can't create coreDstDir %s : %s\n",
                dst, strerror( errno ) );
        return false;
    }
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool core_moveCore( char *src, char *dst )
{
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    int status;

    sprintf( cmd, "/bin/mv %s %s", src, dst );
    status = system( cmd );
    if( -1 == status || 0 != WEXITSTATUS( status ) ) {
        dbg_error("srph: Unable to move [%s] to [%s]: [%s]\n",
                src, dst, strerror( errno ) );
        return false;
    }
    return true;
}


/* TODO: eliminate static arrays and the use of system */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static bool core_moveDebugs( const char *srcDir, char *dst )
{
    DIR *dir;
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    struct dirent *entry;
    int ret;

    if( !( dir = opendir( srcDir ) ) ) {
        dbg_error("srph: Can't open dbgSrcDir [%s] : %s\n",
                srcDir, strerror( errno ) );
        return false;
    }
    while( ( entry = readdir( dir ) ) ) {
        if( !strcmp( entry->d_name, "." )
        ||  !strcmp( entry->d_name, ".." ) )
            continue;
        sprintf( src, "%s/%s", srcDir, entry->d_name );
        sprintf( cmd, "/bin/mv %s %s", src, dst );
        ret = system( cmd );
        if( -1 == ret || 0 != WEXITSTATUS(ret) )
        {   dbg_error("srph: Unable to move [%s] to [%s] : [%s]\n",
                        src, dst, strerror(errno));
            closedir(dir);
            return false;
        }
    }
    closedir( dir ) ;
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* TODO: no more system */
static bool core_moveLog( const char *workDir, char *dst )
{
    char src[FILENAME_MAX] = { 0 };
    char cmd[2 * FILENAME_MAX + 32] = { 0 };
    sprintf( src, "%s/log/default.txt", workDir );
    sprintf( cmd, "/bin/mv %s %s", src, dst );
    system( cmd );
    return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* TODO: no more system */
static bool core_copyCfg( const char *cfgFile, char *dst )
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
core_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
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

            rc = core_setupDstDir( dst, core->d_name,
                    core_srcDir, core_dstDir );
            if( !rc )
                return false;

            if( !core_moveCore(src, dst)
            ||  !core_moveDebugs(dbg_srcDir, dst)
            ||  !core_moveLog(workDir, dst)
            ||  !core_copyCfg(cfgFile, dst)
            )
                return false;

            return true;
        }
    }
    if( -1 == closedir( dir ) ) {
        dbg_error("Unable to close dir [%s]: [%s]\n",
                core_srcDir, strerror( errno ) );
        return false;
    }
    return false;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int core_runBat( const char *bat_name, int timeout )
{
    char c;
    int pid = 0, hasAlarm, ictest;
    struct sigaction act;
    char ctest[33]={0};
    struct Class* db;
    printf( "\n\n" );
    printf( "*-------------------------.\n" );
    printf( "* srph:   Running script  :[%s]\n", bat_name );
    printf( "*-------------------------'\n" );

    act.sa_handler = sig_handler;
    act.sa_flags = SA_NOCLDSTOP;
    sigaction( SIGCHLD, &act, 0 );

    db = new(BaseDB);
    db_open( db, getenv( SUT_ERRLOG) ); //TODO: no more getenv
    db_get( db, "ctest", &ctest);
    ictest = atoi(ctest);
    ++ictest;
    sprintf(ctest, "%d", ictest);
    db_put( db, "ctest", ctest);
    db_close();
    delete(db);

    if( ( pid = fork(  ) ) == 0 ) {
        int rc = 0;
        rc = system( bat_name );
        exit( WEXITSTATUS(rc) );
    } else if( pid > 0 ) {
        while( 1 ) {
            sleep( cfg.script_tout );
            if( waitpid( pid, &hasAlarm, WNOHANG ) )
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
                    dbg_error( "srph: Unable to kill child script: [%s]\n"
                            ,strerror(errno));
                }
                break;
            }
        }
    } else
        dbg_error( "srph: fork() failed" );
    return 0;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static int
core_fileAction( const char *fileName, struct stat *statbuf, void *junk )
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
        return true;
    sut_refresh( cfg.test_type, cfg.sutDefWD,
                 cfg.axi_workDir, cfg.hostname,cfg.rawport );
    core_setupTmp( fileName, tmpDir );
    if( NULL == getcwd( curDir, FILENAME_MAX ) ) {
        dbg_error( "srph: Unable to get current directory: [%s]"
                , strerror(errno));
    }

    if( -1 == chdir( tmpDir ) ) {
        dbg_error("srph: Unable to change directory to [%s]:[%s]\n",
                tmpDir, strerror( errno ) );
        return false;
    }
    core_runBat( fileName, cfg.script_tout );
    core_checkCore( cfg.test_type, cfg.axi_coreDir, cfg.axi_dbgDir,
            cfg.axi_workDir, cfg.axi_cfgFile, cfg.dest_coreDir );
    core_cleanupTmp( tmpDir );
    sleep( 1 );
    if( -1 == chdir( curDir ) ) {
        dbg_error("srph: Unable to change directory to [%s]:[%s]\n",
                curDir, strerror( errno ) );
        return false;
    }

    return ( true );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    static int
core_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
    return ( true );
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int core_runRecursive( const char *srcName )
{
    if( recursiveAction( srcName, 1, false,
                true, core_fileAction, core_dirAction,
                NULL ) == false ) {
        dbg_error( "runRecursive: [%s]\n",strerror(errno) );
        exit( EXIT_FAILURE );
    }
    return true;
}



/*!
 * Copies source_bat in /tmp/PID, to create a sandbox for the test.
 * \param source_file[in] An absolute path pointing to a bat file.
 * \todo  Implement a core_refresh function
 * \todo  Return Bool ??
 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int core_setupTmp( char const *source_bat, char *tmpDir )
{
    char cmd[FILENAME_MAX] = { 0 };
    int status;
    char *s = strdup( source_bat );

    sprintf( tmpDir, "/tmp/%d", getpid(  ) );
    if( -1 == mkdir( tmpDir, 0755 ) ) {
        dbg_error("srph: Can't make directory [%s]: [%s]\n",
                tmpDir, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    dbg_verbose("Copying [%s] to [%s]\n", s, tmpDir );
    sprintf( cmd, "/bin/cp -R %s/* %s/", dirname( s ), tmpDir );

    status = system( cmd );
    free( s );
    s = strdup( source_bat );
    if( -1 == status || 0 != WEXITSTATUS( status ) ) {
        dbg_error("srph: Unable to copy [%s] to [%s]: [%s]\n",
                dirname( s ), tmpDir, strerror( errno ) );
    }
    free( s );
    return 0;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int core_cleanupTmp( const char *tmpDir )
{
    dbg_verbose( "Removing [%s]\n", tmpDir );
    fop_rm( TEST_LOCAL, tmpDir, 0, 0 );
    return 0;
}



/** Search a file for axi_fi=y or axi_fi=n . */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int core_parseBat( const char *filename )
{
#if 0
    char line[PATH_MAX] = { 0 };
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
    while( ( str = fgets( line, PATH_MAX - 1, f ) ) ) {
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





/**
 * \brief search and expand any occurence in str, of ${VARNAME} */
/* expand ${VAR} from prefix/${VAR}/suffix */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char* core_expandVars( const char *t1 )
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
                dbg_error( "Error expanding [%s]\n", text );
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
                dbg_error( "Unmatched bracket in [%s]\n", text );
                return 0;
            }
            p++;
            if( NULL == getenv( id ) ) {
                dbg_error( "Can't expand [%s] : [Not exported]\n", id );
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

static int testCounter;

static int
countTests_fileAction( const char* fileName, struct stat* statbuf, void* junk)
{
        if( strcmp( basename(fileName), "runtest.bat") )
            return ( false );
        if( statbuf->st_mode & S_IXGRP | S_IXOTH | S_IXUSR )
        {
            ++testCounter;
            debug("found runtest.bat\n");
            return ( true );
        }

        return ( false );
}

static int countTests( const char* const path)
{
        struct stat statbuf;
        //recursiveFlag = 1;
        //forceFlag = false;
        #if 0
        if( lstat( path, &statbuf ) != 0 && errno == ENOENT ) {
            /* do not reports errors for non-existent files if -f, just skip them */
        } else
#endif
        {
            if( recursiveAction( path, 1, false,
                        true, countTests_fileAction, NULL,
                        NULL ) == false ) {
                exit( false );
            }
        }
        return testCounter;
    return 0;
}

/*
 * Export axi_errorlog=$CWD/errors/$HOST-$PID
 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int core_setErrorlog( const char* const uname, const char* const test_dir )
{
    time_t now;
    struct tm *tm = 0;
    struct Class* db=NULL;
    char *dupname=NULL, *fname=NULL, *machine=NULL, *dir=NULL;

    machine = strrchr( cfg.config_file, '/');
    fname = (char*)malloc( strlen(USERDB) + strlen(uname) + strlen(machine) + 24);
    sprintf( fname, "%s/%s/jobs/running%s-%d", USERDB, uname, machine, getpid()) ;
    dbg_verbose( "LogFile is: [%s]\n", fname );

    /* Create target directory in case it doesn't exists.
     * This should not be the case if this is a registred user */
    dupname = strdup( fname);
    dir = dirname( dupname);
    if( -1 == access( dir, F_OK ) )
    {   dbg_error( "[%s] doesn't exist\n", dir );
        if( -1 == mkdir( dir, 0777 ) )
        {   dbg_error( "Unable to create [%s]: [%s]\n", dir, strerror(errno) );
            free( dupname);
            free( fname);
            return false;
        }
    }
    dbg_verbose( "Directory [%s] created\n", dir );
    fclose(fopen(fname, "w"));
    setenv( SUT_ERRLOG, fname, 1 );

    db = new(BaseDB);
    time( &now );
    tm = localtime( &now );
    db_open( db, "%s", fname);

    sprintf( fname, "%d-%02d-%02d", 1900+tm->tm_year, tm->tm_mon, tm->tm_mday );
    db_put( db, "date", fname);

    sprintf( fname, "%02d.%02d.%02d", tm->tm_hour, tm->tm_min, tm->tm_sec );
    db_put( db, "time", fname);

    sprintf( fname, "%d", countTests(test_dir) );
    db_put( db, "tests", fname);
    db_put( db, "ctest", "0");
    db_close( db );
    delete(db);
    free( dupname);
    free( fname);
    return true;
}




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int core_runTests( const char *dir )
{
    struct stat inf;
    char fullPath[FILENAME_MAX] = { 0 };
    char curDir[FILENAME_MAX] = { 0 };

    if( stat( dir, &inf ) < 0 ) {
        dbg_error( "srph: [%s] : [%s].\n", dir, strerror(errno) );
        return 0;
    }
    dbg_verbose("Running tests from [%s]\n", dir);
    getcwd( curDir, FILENAME_MAX );
    if( -1 == chdir( dir ) ) {
        dbg_error("srph: Can't change to [%s] : [%s]\n",
                dir, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    /*! @todo verify if `tests` dir is an absolute or a relative path */
    //sprintf( fullPath, "%s/%s", curDir, dir );
    sprintf( fullPath, "%s", dir );
    core_runRecursive( fullPath );
    return true;
}

