/**
 *   \brief    
 *   \author   Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "socket.h"
#include "sut.h"


struct config_s cfg;
char* axi_param = NULL ;
static int rc_parseArgs( int argc, char *argv[] );

static void
stop_usage( int status );

int
main( int argc, char *argv[] )
{
        char *tc;
        char *path;
        int test_type ;

        rc_parseArgs( argc, argv );
        str_isEnv( cfg.verbose, SUT_TTYPE );
        tc = getenv( SUT_TTYPE );

        if( ! axi_param ) {
            fprintf(stderr, "! stop: -c flag is mandatory\n");
            stop_usage( EXIT_FAILURE ) ;
        }
        //! @todo replace 5
        if( !strcasecmp( tc, "local" ) ) {
                printf( "* stop: Working Local\n" );
                test_type = TEST_LOCAL ;
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* stop: Working remote\n" );
                str_isEnv( cfg.verbose, SUT_HOST );
                str_isEnv( cfg.verbose, SUT_PORT );

                cfg.hostname = getenv( SUT_HOST );
                cfg.port     = atoi( getenv( SUT_PORT ) );
                test_type    = TEST_REMOTE ;
        } else {
                printf( "! stop : Invalid $axi_ttype\n" );
                return EXIT_FAILURE;
        }
        sut_stop( test_type, 5, getenv(SUT_SYSLOG),axi_param ,0,0);
        return EXIT_SUCCESS;
}

static void
stop_usage( int status )
{

        printf( "Usage: start [OPTION] COMMAND...\n" );
        printf( "Stop Axigen.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n");
        printf( "  -P port\n");
        printf( "  -c params\n");
        printf( "  -t testType\n");

        exit( status );
}



static int
rc_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:c:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" ) ) {
                            setenv( "axi_ttype", optarg, 1 );
                            cfg.test_type = TEST_REMOTE ;
                        }
                        if( !strcasecmp( optarg, "local" ) )  {
                            setenv( "axi_ttype", optarg, 1 );
                            cfg.test_type = TEST_LOCAL ;
                        }
                        break;
                case 'H':
                        setenv( "axi_host", optarg, 1 );
                        cfg.hostname = optarg ;
                        break;
                case 'P':
                        setenv( "axi_port", optarg, 1 );
                        cfg.port = atoi(optarg);
                        break;
                case 'c':
                        axi_param = optarg ;
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
