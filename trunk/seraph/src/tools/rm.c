/**
 *   \brief    Wraps around rm command.
 *   \see      ptgenhost.c 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"
#include "fileop.h"

struct config_s cfg;
static int rm_parseArgs( int argc, char *argv[] );

int main( int argc, char *argv[] )
{
        char *tc=NULL;               // conexion type
        char *host=NULL;
        int port=0, test_type=TEST_LOCAL;

        if( argc < 2 )          // trebuie sa am sursa si destinatie
        {
                printf( "! rm: missing file operand\n" );
                printf( "Try `rm -h` for more information.\n" );
                return 1;
        }
        rm_parseArgs( argc, argv );
        str_isEnv( SUT_TTYPE );
        tc = getenv( SUT_TTYPE );
        if( argc < 2 ) {
                fprintf( stderr, "! mkdir: invalid syntax" );
                return 1;
        }

        if( !strcmp( tc, "local" ) )
                test_type = TEST_LOCAL;
        else if( !strcmp( tc, "remote" ) )
                test_type = TEST_REMOTE;
        else
                printf( "! mkdir: Invalid $axi_ttype\n" );
        fop_rm( test_type, argv[optind], host, port );
        return 1;
}



void rm_usage( int status )
{

        printf( "Usage: rm [OPTION] FILE...\n" );
        printf( "Remove (unlink) the FILE(s).\n" );
        printf( "\n" );
        printf
            ( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n" );
        printf( "  -P port\n" );
        printf( "  -t testType\n" );
        exit( status );
}



static int rm_parseArgs( int argc, char *argv[] )
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
                        rm_usage( EXIT_SUCCESS );
                case 'v':
                        cfg.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
