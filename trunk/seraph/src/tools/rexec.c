/**
 *   \brief  execute a given parameter, either on local machine, or remote
 *   \see    socket.c
 *   \date Thu Aug 17 17:38:13 2006
 *
 */
/*#define _POSIX_SOURCE*/

#include <stdlib.h>
#include <wait.h>
#include <strings.h>
#include <unistd.h>
#include "config.h"
#include "socket.h"
#include "sut.h"
#include "strop.h"
#include "fileop.h"

static int rex_parseArgs( int argc, char *argv[] );
struct config_s cfg;

extern char *optarg;
extern int optind;

static int cli_main( char *host, int port, char *c )
{
        int cod, sockfd;
        char cmd[LINE_MAX] = { 0 };

         /*REMOTE*/ sockfd = sock_connectTo( host, port );

        sprintf( cmd, "EXECUTE %s", c );
        printf( "[%s]\n", cmd );
        sock_sendLine( sockfd, cmd );
        cod = sock_getStatus( sockfd );
        if( cod < 0 ) {
                fputs( "! rexec: No remote confirmation!\n", stderr );
                return errno;
        }
        if( cod > 0 ) {
                fprintf( stderr, "! rexec: Error: '%s'\n", strerror( cod ) );
                fprintf( stderr, "! rexec: Can't execute : '%s'\n", c );
                close( sockfd );
                return 1;
        }
        close( sockfd );
        return 0;
}


int main( int argc, char *argv[] )
{
        char *hostname = 0, *ttype = 0;
        int port = 0;
        int cod = -1;
        int verbose = 1;
        if( argc < 2 ) {
                printf( "! rexec: missing operand\n" );
                printf( "Try `rexec -h` for more information.\n" );
                return EXIT_FAILURE;
        }
        rex_parseArgs( argc, argv );
        if( signal( SIGINT, sut_sigint ) == SIG_ERR ) {
                perror( "! rexec: Signal error!" );
                return EXIT_FAILURE;
        }

        str_isEnv( SUT_TTYPE );
        ttype = getenv( SUT_TTYPE );

         /*LOCAL*/ if( !strcasecmp( ttype, "local" ) ) {
                cod = system( argv[optind] );
                exit( WEXITSTATUS( cod ) );
        } else if( !strcasecmp( ttype, "remote" ) ) {
                str_isEnv( SUT_HOST );
                str_isEnv( SUT_PORT );
                hostname = getenv( SUT_HOST );
                port = atoi( getenv( SUT_PORT ) );
                cod = cli_main( hostname, port, argv[optind] );
        } else
                printf( "* rexec: Invalid test type\n" );

        return -2;
}

void rex_usage( int status )
{

        printf( "Usage: rexec [OPTION] COMMAND...\n" );
        printf( "Send a COMMAND to the seraph daemon\n" );
        printf( "\n" );
        printf
            ( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname       Host to connect to\n" );
        printf( "  -P port           Port\n" );
        printf( "  -t testType\n" );
        exit( status );
}
static int rex_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" )
                            || ( !strcasecmp( optarg, "local" ) ) )
                                setenv( "axi_ttype", optarg, 1 );
                        break;
                case 'H':
                        setenv( "axi_host", optarg, 1 );
                        break;
                case 'P':
                        setenv( "axi_port", optarg, 1 );
                        break;
                case 'h':
                        rex_usage( EXIT_SUCCESS );
                case 'v':
                        break;
                }
        }
        return TRUE;
}
