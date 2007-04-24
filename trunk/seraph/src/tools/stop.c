/**
 *   \brief    
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"


struct config_s cfg;
char *stop_cmd = NULL;
static int rc_parseArgs( int argc, char *argv[] );

static void stop_usage( int status );

int main( int argc, char *argv[] )
{
        char *tc;
        int test_type;

        rc_parseArgs( argc, argv );
        str_isEnv( SUT_TTYPE );
        tc = getenv( SUT_TTYPE );

        if( !stop_cmd ) {
                fprintf( stderr, "E: stop: -c flag is mandatory\n" );
                stop_usage( EXIT_FAILURE );
        }
        //! @todo replace 5
        if( !strcasecmp( tc, "local" ) ) {
                printf( "* stop: Working Local\n" );
                test_type = TEST_LOCAL;
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* stop: Working remote\n" );
                str_isEnv( SUT_HOST );
                str_isEnv( SUT_PORT );

                cfg.hostname = getenv( SUT_HOST );
                cfg.port = atoi( getenv( SUT_PORT ) );
                test_type = TEST_REMOTE;
        } else {
                printf( "E: stop : Invalid test type $SUT_TTYPE\n" );
                return EXIT_FAILURE;
        }
        sut_stop( test_type, 5, getenv( SUT_SYSLOG ), stop_cmd, cfg.hostname,
                  cfg.port );
        return EXIT_SUCCESS;
}

static void stop_usage( int status )
{

        printf( "Usage: start [OPTION] COMMAND...\n" );
        printf( "Stop.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n" );
        printf( "  -P port\n" );
        printf( "  -c params\n" );
        printf( "  -t testType\n" );

        exit( status );
}



static int rc_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:c:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" ) ) {
                                setenv( SUT_TTYPE, optarg, 1 );
                                cfg.test_type = TEST_REMOTE;
                        }
                        if( !strcasecmp( optarg, "local" ) ) {
                                setenv( SUT_TTYPE, optarg, 1 );
                                cfg.test_type = TEST_LOCAL;
                        }
                        break;
                case 'H':
                        setenv( SUT_HOST, optarg, 1 );
                        cfg.hostname = optarg;
                        break;
                case 'P':
                        setenv( SUT_PORT, optarg, 1 );
                        cfg.port = atoi( optarg );
                        break;
                case 'c':
                        stop_cmd = optarg;
                        break;
                case 'h':
                        stop_usage( EXIT_SUCCESS );
                case 'v':
                        cfg.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
