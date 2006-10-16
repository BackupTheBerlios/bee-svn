#include    "util.h"
#include    "socket.h"

extern int sockfd;
extern struct globals_s glob;
#define LINE_SIZE 8191

static int recursiveFlag = 0;
static int forceFlag = 0;

int wait_start( int timeout );
int wait_stop( int timeout );



/*
*/
int
util_ptStart( int test_type, int timeout, char *start )
{
        //int rc =-1;
        printf( "* Starting ... %s\n", getenv( PT_START ) );
        if( test_type == TEST_LOCAL ) {
                util_ptStartLocal( timeout );
                sleep( 1 );     // still need to wait a little
                printf( "OK\n" );
        } else {
                int sockfd = -1;
                sockfd = sock_connectTo( glob.hostname, glob.port );
                sock_sendLine( sockfd, "START 5" );     // hardcoded timeout
                printf( "OK\n" );
        }
        return TRUE;
}




/**
 * \brief wait until is started
 * \todo Replace hardcoded path to maillog to an ENV path
 * \todo make it work remote too
 * Will read from maillog file, in a manner simmilar to 
 * tail -f, looks for SUCCESS: supervise ready, and INFO: ready
 * \warning WORKS LOCALLY ONLY.
 */
static int
util_ptStartLocal( int timeout )
{
#define supRL 24
#define ptRL 11
        int fd = 0, i = 0, rc = 0, newsz = 0, oldsz = 0;
        char buf[512] = { 0 };
        char *supRdyStr = "SUCCESS: supervise ready";
        char *rdyStr = "INFO: ready";
        int supRdy = FALSE;

        printf( "Waiting\n* LOG\n++++++++++\n" );
        oldsz = util_fileSize( "/var/log/maillog" );
        fd = open( "/var/log/maillog", O_RDONLY );
        //lseek(fd, 0, SEEK_END);
        oldsz = util_fileSize( "/var/log/maillog" );
        rc = system( getenv( PT_START ) );
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                // ask for options
                return FALSE;
        }
        for( ;; ) {
                /*newsz = util_fileSize( "/var/log/maillog" );
                   if( newsz == oldsz )
                   continue ; */
                oldsz = newsz;
                memset( buf, '\0', 512 );
                i = read( fd, buf, sizeof( buf ) - 1 );
                if( i < 0 ) {
                        fprintf( stderr, "Cant read maillog\n" );
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
                                rc = util_strsrch( buf, i, supRdyStr, supRL );
                                supRdy = rc;
                                if( rc )
                                        printf( "////////Found SUPERVISER/////\n" );
                        } else {
                                rc = util_strsrch( buf, i, rdyStr, ptRL );
                                if( rc ) {
                                        printf( "----------\n" );
                                        close( fd );
                                        return TRUE;
                                }
                        }
                }
        }
        close( fd );
#undef supRL
#undef ptRL
        return 1;
}

static int
util_ptStartRemote(  )
{
}


int
util_ptStop( int test_type, int timeout, char *stop )
{
        printf( "* Stoping ...\n" );
        if( glob.ttype == TEST_LOCAL )
                util_ptStopLocal( timeout );
        if( glob.ttype == TEST_REMOTE )
                printf( "* stopped\n" );
        return 0;
}

static int
util_ptStopLocal( int timeout )
{
#define supRL 30
#define ptRL 25
        int fd = 0, i = 0, rc = 0;      //, newsz=0, oldsz=0;
        char buf[512] = { 0 };
        char *supRdyStr = "INFO: supervise: caught signal";
        char *rdyStr = "INFO: supervise: finished";
        int supRdy = FALSE;

        printf( "Waiting\n* LOG\n++++++++++\n" );
        //oldsz = util_fileSize( "/var/log/maillog" );
        fd = open( "/var/log/maillog", O_RDONLY );
        lseek( fd, 0, SEEK_END );
        //oldsz = util_fileSize( "/var/log/maillog" );
        rc = system( getenv( PT_STOP ) );      //! @todo replace getenv with a parameter 
        if( rc == -1 ) {
                printf( "Failed\n" );
                exit( EXIT_FAILURE );
                // ask for options
                return FALSE;
        }
        for( ;; ) {
                /*newsz = util_fileSize( "/var/log/maillog" );
                   if( newsz == oldsz )
                   continue ;
                   oldsz = newsz ; */
                memset( buf, '\0', 512 );
                i = read( fd, buf, sizeof( buf ) - 1 );
                if( i < 0 ) {
                        fprintf( stderr, "Cant read maillog\n" );
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
                                rc = util_strsrch( buf, i, supRdyStr, supRL );
                                supRdy = rc;
                                if( rc )
                                        printf( "////////Found SUPERVISER/////\n" );
                        } else {
                                rc = util_strsrch( buf, i, rdyStr, ptRL );
                                if( rc ) {
                                        printf( "----------\n" );
                                        close( fd );
                                        return TRUE;
                                }
                        }
                }
        }
#undef supRL
#undef ptRL
        return 1;
}


int
util_fileSize( char *name )
{
        struct stat inf;

        if( stat( name, &inf ) < 0 ) {
                fprintf( stderr, "stat() error: :%s %s\n", name,
                         strerror( errno ) );
                exit( -1 );
        }
        return inf.st_size;
}



void
util_terminare(  )
{
        printf( "Bailing out\n" );
        exit( 1 );
}

int
util_strsrch( const char *hay_stack, int hay_size, const char *needle,
              int needle_size )
{
        int i, j;

        j = needle_size - 1;
        for( i = hay_size - 1; i >= 0; --i ) {
                if( hay_stack[i] == needle[j] ) {
                        if( --j == -1 ) {       /*printf("Line matched\n"); */
                                return TRUE;
                        }
                        continue;
                } else {
                        j = needle_size - 1;
                }
        }
        return FALSE;
}



int
util_isEnv( char *var_name )
{
        if( !getenv( var_name ) ) {
                fprintf( stderr, "$%s not exported.\n", var_name );
                exit( -1 );
        }
        if( glob.verbose == TRUE )
                printf( "$%s=%s\n", var_name, getenv( var_name ) );
        return 0;
}



int
util_startsWith( char *str, char *exp )
{
        int i;

        for( i = 0; i < strlen( exp ); ++i ) {
                if( str[i] != exp[i] )
                        return 0;
        }
        return 1;
}


int
client_rm( char *host, int port, char *path )
{
        int cod = 0;
        char cmd[MAX_LIN] = { 0 };
        char str[MAX_LIN] = { 0 };
        int sockfd = -1;
        sockfd = sock_connectTo( host, port );
        if( sockfd == -1 )
                exit( -1 );
        sprintf( cmd, "RM %s", path );
        write( sockfd, cmd, strlen( cmd ) );    //TODO: check for error
        read( sockfd, str, 8191 );
        cod = util_getCode( str );
        if( cod ) {
                fprintf( stderr, "Error, could not delete the path %s: %s\n",
                         path, strerror( cod ) );
                close( sockfd );
                return cod;
        }
        close( sockfd );
        return 0;

}


int
util_matches( char *buffer, char *string )
{
        if( !strncasecmp( buffer, string, strlen( string ) ) )
                return 1;
        return 0;
}




/*Copyright busybox*/
/* 
 * Walk down all the directories under the specified 
 * location, and do something (something specified
 * by the fileAction and dirAction function pointers).
 *
 * Unfortunatly, while nftw(3) could replace this and reduce 
 * code size a bit, nftw() wasn't supported before GNU libc 2.1, 
 * and so isn't sufficiently portable to take over since glibc2.1
 * is so stinking huge.
 */
int
recursiveAction( const char *fileName,
                 int recurse, int followLinks, int depthFirst,
                 int ( *fileAction ) ( const char *fileName,
                                       struct stat * statbuf,
                                       void *userData ),
                 int ( *dirAction ) ( const char *fileName,
                                      struct stat * statbuf,
                                      void *userData ), void *userData )
{
        int status;
        struct stat statbuf;
        struct dirent *next;

        if( followLinks == TRUE )
                status = stat( fileName, &statbuf );
        else
                status = lstat( fileName, &statbuf );

        if( status < 0 ) {
#ifdef BB_DEBUG_PRINT_SCAFFOLD
                fprintf( stderr,
                         "status=%d followLinks=%d TRUE=%d\n",
                         status, followLinks, TRUE );
#endif
                perror( fileName );
                return FALSE;
        }

        if( ( followLinks == FALSE ) && ( S_ISLNK( statbuf.st_mode ) ) ) {
                if( fileAction == NULL )
                        return TRUE;
                else
                        return fileAction( fileName, &statbuf, userData );
        }

        if( recurse == FALSE ) {
                if( S_ISDIR( statbuf.st_mode ) ) {
                        if( dirAction != NULL )
                                return ( dirAction
                                         ( fileName, &statbuf, userData ) );
                        else
                                return TRUE;
                }
        }

        if( S_ISDIR( statbuf.st_mode ) ) {
                DIR *dir;

                dir = opendir( fileName );
                if( !dir ) {
                        perror( fileName );
                        return FALSE;
                }
                if( dirAction != NULL && depthFirst == FALSE ) {
                        status = dirAction( fileName, &statbuf, userData );
                        if( status == FALSE ) {
                                perror( fileName );
                                return FALSE;
                        }
                }
                while( ( next = readdir( dir ) ) != NULL ) {
                        char nextFile[BUFSIZ + 1];

                        if( ( strcmp( next->d_name, ".." ) == 0 )
                            || ( strcmp( next->d_name, "." ) == 0 ) ) {
                                continue;
                        }
                        if( strlen( fileName ) + strlen( next->d_name ) + 1 >
                            BUFSIZ ) {
                                fprintf( stderr, "Name too long %s", "ftw" );
                                return FALSE;
                        }
                        memset( nextFile, 0, sizeof( nextFile ) );
                        sprintf( nextFile, "%s/%s", fileName, next->d_name );
                        status = recursiveAction( nextFile, TRUE, followLinks,
                                                  depthFirst, fileAction,
                                                  dirAction, userData );
                        if( status < 0 ) {
                                closedir( dir );
                                return FALSE;
                        }
                }
                status = closedir( dir );
                if( status < 0 ) {
                        perror( fileName );
                        return FALSE;
                }
                if( dirAction != NULL && depthFirst == TRUE ) {
                        status = dirAction( fileName, &statbuf, userData );
                        if( status == FALSE ) {
                                perror( fileName );
                                return FALSE;
                        }
                }
        } else {
                if( fileAction == NULL )
                        return TRUE;
                else
                        return fileAction( fileName, &statbuf, userData );
        }
        return TRUE;
}




static int
rm_fileAction( const char *fileName, struct stat *statbuf, void *junk )
{
        if( unlink( fileName ) < 0 ) {
                perror( fileName );
                return ( FALSE );
        }
        return ( TRUE );
}


static int
rm_dirAction( const char *fileName, struct stat *statbuf, void *junk )
{
        if( rmdir( fileName ) < 0 ) {
                perror( fileName );
                return ( FALSE );
        }
        return ( TRUE );
}


int
my_rm( char *srcName )
{
        struct stat statbuf;

        recursiveFlag = 1;
        forceFlag = TRUE;
        if( forceFlag == TRUE && lstat( srcName, &statbuf ) != 0
            && errno == ENOENT ) {
                /* do not reports errors for non-existent files if -f, just skip them */
        } else {
                if( recursiveAction( srcName, recursiveFlag, FALSE,
                                     TRUE, rm_fileAction, rm_dirAction,
                                     NULL ) == FALSE ) {
                        exit( FALSE );
                }
        }
        return 1;
}

int
util_endsWith( const char *str, const char *model )
{
        int len_str = strlen( str );
        int len_m = strlen( model );
        int i;

        for( i = 1; i <= len_m; ++i ) {
                if( str[len_str - i] != model[len_m - i] )
                        return 0;
        }
        return 1;
}

int
util_getCode( char *str )
{
        if( !util_startsWith( str, "OK:" )
            && !util_startsWith( str, "ERR:" ) ) {
                fprintf( stderr, "The string received from host is '%s'\n",
                         str );
                fprintf( stderr, "Expecting OK: or ERR:\n" );
                exit( 1 );
        }
        printf( "* Received : `%s`\n", str );
        char *b = strchr( str, ':' );
        int i = 1;

        while( isdigit( b[i] ) )
                ++i;
        if( i == 1 )
                return 0;       // nu avem nici o cifra
        char cod[MAX_LIN];

        memcpy( cod, b + 1, i - 1 );
        cod[i - 1] = '\0';
        return atoi( cod );
}





/**
 * \brief Check to see if a core was droped.
 * \return FALSE if core not dropped.
 * For the moment, this works local only.
 * Have to implement the same function in host.c*/
int
util_checkCoreLocal( const char *core_srcDir, const char *dbg_srcDir,
                     const char *workDir, const char *cfgFile,
                     const char *crash_destDir )
{
        DIR *dir;
        struct dirent *entry, *core;
        int rc = 0;
        char src[PATH_MAX] = { 0 };
        char dst[PATH_MAX] = { 0 };
        char cmd[2 * PATH_MAX + 32] = { 0 };


        dir = opendir( core_srcDir );
        if( !dir ) {
                fprintf( stderr, "Can't open %s : %s\n", core_srcDir,
                         strerror( errno ) );
                exit( -1 );
        }

        while( ( core = readdir( dir ) ) ) {
                if( strstr( core->d_name, "core" ) ) {
                        sprintf( dst, "%s/data/crash-%s", crash_destDir,
                                 core->d_name );
                        rc = mkdir( dst, 0777 );
                        if( rc == -1 ) {
                                fprintf( stderr,
                                         "Can't create core folder %s/data/crash-%s\n",
                                         glob.testbot_path, core->d_name );
                                return TRUE;
                        }
                        sprintf( src, "%s/%s", glob.pt_coreDir, core->d_name );

                        // 1. Move CORE
                        sprintf( cmd, "/bin/mv %s %s", src, dst );
                        system( cmd );
                        closedir( dir );

                        // 2. Move DEBUGLOGS
                        dir = opendir( dbg_srcDir );
                        if( !dir ) {
                                fprintf( stderr, "Can't open %s : %s\n",
                                         glob.pt_dbgDir, strerror( errno ) );
                                exit( -1 );
                        }
                        while( ( entry = readdir( dir ) ) ) {
                                if( !strcmp( entry->d_name, "." )
                                    || !strcmp( entry->d_name, ".." ) )
                                        continue;
                                sprintf( src, "%s/%s", glob.pt_dbgDir,
                                         entry->d_name );
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
