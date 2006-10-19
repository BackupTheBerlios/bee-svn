/**
 *   \brief    
 *   \author   Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "util.h"
#include "socket.h"


struct globals_s glob;
char* axi_param;
static int rc_parseArgs( int argc, char *argv[] );


int
main( int argc, char *argv[] )
{
        char *tc;
        char *path;

        rc_parseArgs( argc, argv );
        util_isEnv( AXI_TTYPE );
        tc = getenv( AXI_TTYPE );

        if( !strcasecmp( tc, "local" ) ) {
                printf( "* stop: Working local\n" );
                //! @todo replace 5
                util_axiStop( TEST_LOCAL, 5, axi_param ,0,0);
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* stop: Working remote\n" );
                util_isEnv( AXI_HOST );
                util_isEnv( AXI_PORT );

                glob.hostname = getenv( AXI_HOST );
                glob.port = atoi( getenv( AXI_PORT ) );
                path = getenv( AXI_WORKDIR );

                //! @todo replace 5
                util_axiStop( TEST_REMOTE, 5, axi_param ,
                                glob.hostname, glob.port );
        } else {
                printf( "* stop : Invalid $axi_ttype\n" );
                return 1;
        }
        return EXIT_SUCCESS;
}

void
rc_usage( void )
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
                            glob.test_type = TEST_REMOTE ;
                        }
                        if( !strcasecmp( optarg, "local" ) )  {
                            setenv( "axi_ttype", optarg, 1 );
                            glob.test_type = TEST_LOCAL ;
                        }
                        break;
                case 'H':
                        setenv( "axi_host", optarg, 1 );
                        glob.hostname = optarg ;
                        break;
                case 'P':
                        setenv( "axi_port", optarg, 1 );
                        glob.port = atoi(optarg);
                        break;
                case 'c':
                        axi_param = optarg ;
                        break;
                case 'h':
                        rc_usage(  );
                case 'v':
                        glob.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
