#include    "strop.h"
#include    "fileop.h"
#include    "socket.h"

extern int sockfd;
extern struct cfgals_s cfg;
#define LINE_SIZE 8191

static int recursiveFlag = 0;
static int forceFlag = 0;

static int sut_axiStartLocal( int timeout, char* start );
static int sut_axiStopLocal( int timeout, char* stop );




int
fop_fileSize( char *name )
{
        struct stat inf;

        if( stat( name, &inf ) < 0 ) {
                fprintf( stderr, "stat() error: :%s %s\n", name,
                         strerror( errno ) );
                exit( -1 );
        }
        return inf.st_size;
}


int
fop_rm( char *srcName )
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

/*
int
fop_rm( int test_type, char* path, char* host, int port )
{
    if(test_type == TEST_LOCAL)
        return fop_rmLocal( path );
    if(test_type == TEST_REMOTE)
        return fop_rmRemote( path, host, port );
    return FALSE ;
}
*/
static int
fop_rmRemote( char* path, char* host, int port )
{
}

static int
fop_rmLocal( char *path )
{
        struct stat statbuf;
        recursiveFlag = 1;
        forceFlag = TRUE;
        if( forceFlag == TRUE && lstat( path, &statbuf ) != 0
            && errno == ENOENT ) {
                /* do not reports errors for non-existent files if -f, just skip them */
        } else {
                if( recursiveAction( path, recursiveFlag, FALSE,
                                     TRUE, rm_fileAction, rm_dirAction,
                                     NULL ) == FALSE ) {
                        exit( FALSE );
                }
        }
        return 1;
}







/*-------------*/


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
