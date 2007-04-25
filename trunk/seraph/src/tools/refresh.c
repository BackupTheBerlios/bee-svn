/**
 *   \brief    Handles the refreshing of ptgen state(users/domains).
 */
#include <strings.h>
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"



struct config_s cfg;
static int rc_parseArgs( int argc, char *argv[] );



int
main( int argc, char *argv[] )
{
        char *tc;
        int test_type = TEST_LOCAL;

        rc_parseArgs( argc, argv );
        str_isEnv( SUT_TTYPE );
        str_isEnv( SUT_STOP );
        str_isEnv( SUT_START );
        str_isEnv( SUT_DEFDOM );
        str_isEnv( SUT_WORKDIR );
        tc = getenv( SUT_TTYPE );

        if( !strcasecmp( tc, "local" ) )
                test_type = TEST_LOCAL;
        else if( !strcasecmp( tc, "remote" ) ) {
                str_isEnv( SUT_HOST );
                str_isEnv( SUT_PORT );
                test_type = TEST_REMOTE;
                cfg.hostname = getenv( SUT_HOST );
                cfg.port = atoi( getenv( SUT_PORT ) );
        } else {
                printf( "E: refresh : Invalid test type $SUT_TTYPE\n" );
                return 1;
        }

        system( getenv( SUT_STOP ) );
        sut_refresh( test_type, getenv( SUT_DEFDOM ),
                     getenv( SUT_WORKDIR ), cfg.hostname, cfg.port );
        system( getenv( SUT_START ) );
        return EXIT_SUCCESS;
}

void rc_usage( int status )
{

        printf( "Usage: refresh [OPTION] COMMAND...\n" );
        printf( "Refresh the state of serverUnderTest.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n" );
        printf( "  -P port\n" );
        printf( "  -t testType\n" );

        exit( status );
}



static int rc_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
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
                case 'h':
                        rc_usage( EXIT_SUCCESS );
                case 'v':
                        cfg.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
