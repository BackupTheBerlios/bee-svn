/**
 *   \brief    
 *   \author   Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "sut.h"
#include "socket.h"

//! @todo use axi_binpath to look for axigen
struct config_s cfg ;
char* axi_param;
static int rc_parseArgs( int argc, char *argv[] );


int
main( int argc, char *argv[] )
{
        char *tc;
        char *path;
        int test_type =TEST_LOCAL;

        rc_parseArgs( argc, argv );
        str_isEnv( cfg.verbose , SUT_TTYPE );
        tc = getenv( SUT_TTYPE );

        if( !strcasecmp( tc, "local" ) )
                printf( "* start: Working Local\n" );
                test_type = TEST_LOCAL ;
        else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* start: Working Remote\n" );
                str_isEnv( cfg.verbose, SUT_HOST );
                str_isEnv( cfg.verbose, SUT_PORT );

                test_type = TEST_REMOTE ;
                cfg.hostname = getenv( SUT_HOST );
                cfg.port = atoi( getenv( SUT_PORT ) );
        } else {
                printf( "* start : Invalid $axi_ttype\n" );
                return 1;
        }
        sut_start( test_type, 5, getenv(SUT_SYSLOG), axi_param ,
                                cfg.hostname, cfg.port );
        return EXIT_SUCCESS;
}

void
rc_usage( void )
{

        printf( "Usage: start [OPTION] COMMAND...\n" );
        printf( "Start Axigen.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n");
        printf( "  -P port\n");
        printf( "  -c params\n");
        printf( "  -t testType\n");

        exit( 0 );
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
                        rc_usage(  );
                case 'v':
                        cfg.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
