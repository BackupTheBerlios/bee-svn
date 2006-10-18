/**
 *   \brief    Handles the refreshing of ptgen state(users/domains).
 *   \see      ptgenhost.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "util.h"
#include "socket.h"


struct globals_s glob;
static int rc_parseArgs( int argc, char *argv[] );


static int
refresh_local( char* source, char* dest )
{
        char cmd[8192] = { 0 } ;
        printf( "* refresh: Removing %s\n", dest );
        sprintf( cmd, "/bin/rm -rf %s", dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: %s not deleted\n", dest );
                return 1;
        }
        printf( "* refresh: copying %s to %s\n", source, dest );
        if( dest[strlen( dest ) - 1] == '/' )
                dest[strlen( dest ) - 1] = '\0';
        sprintf( cmd, "/bin/cp -R %s %s", source, dest );
        if( system( cmd ) ) {
                printf( "* refresh: ERR: File not copied\n" );
                return 1;
        }
        return 0;
}

static int
refresh_remote( char *host, int port, char *sursa, char *dest )
{
        char command[MAX_LIN] = { 0 };
        int cod, sockfd;

        sockfd = sock_connectTo( host, port );

         /*REFRESH*/
        sprintf( command, "REFRESH %s %s", sursa, dest );
        sock_sendLine( sockfd, command );
        cod = sock_getStatus( sockfd );
        if( cod < 0 ) {
                fprintf( stderr, "ERR: %s\n", strerror( cod ) );
                close( sockfd );
                return cod;
        }

        close( sockfd );
        return 0;
}



static int
axi_refresh( int test_type, char *source, char *dest, char *host, int port )     // Cristina
{
        struct stat buf;
        int cod = 0;

        if( test_type == TEST_LOCAL ) {
        	cod = stat( source, &buf );
	        if( cod != 0 ) {
        	        fprintf( stderr, "* refresh: ERR: %s: %s\n", source,
                	         strerror( errno ) );
	                exit( -1 );
        	}
                return refresh_local( source, dest );
        }

        if( test_type == TEST_REMOTE ) {
                if( refresh_remote( host, port, source, dest ) ) {
                        printf( "* refresh: ERR: Could not make refresh!!!\n" );
                        return EXIT_FAILURE;
                }
                return 0;
        }
        return 0;
}


int
main( int argc, char *argv[] )
{
        char *tc;
        char *path;

        rc_parseArgs( argc, argv );
        util_isEnv( AXI_TTYPE );
        util_isEnv( AXI_STOP );
        util_isEnv( AXI_START );
        util_isEnv( AXI_DEFDOM );
        util_isEnv( AXI_WORKDIR );
        tc = getenv( AXI_TTYPE );

        if( !strcasecmp( tc, "local" ) ) {
                printf( "* refresh: Working local\n" );
                util_axiStop( TEST_LOCAL, 5, getenv( AXI_STOP ),0,0 );       //! @todo replace 5 with a proper timeout
                axi_refresh( TEST_LOCAL, getenv( AXI_DEFDOM ),
                            getenv( AXI_WORKDIR ), 0, 0 );
                util_axiStart( TEST_LOCAL, 5, getenv( AXI_START ) ,0,0);     //! @todo replace 5
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* refresh: Working remote\n" );
                util_isEnv( AXI_HOST );
                util_isEnv( AXI_PORT );

                glob.hostname = getenv( AXI_HOST );
                glob.port = atoi( getenv( AXI_PORT ) );
                path = getenv( AXI_WORKDIR );
                util_axiStop( TEST_REMOTE, 5, getenv( AXI_STOP ),
                                glob.hostname, glob.port );
                axi_refresh( TEST_REMOTE, getenv( AXI_DEFDOM ),
                            getenv( AXI_WORKDIR ), glob.hostname, glob.port );
                util_axiStart( TEST_REMOTE, 5, getenv( AXI_START ) ,
                                glob.hostname, glob.port );    //! @todo replace 5
        } else {
                printf( "* refresh : Invalid $axi_ttype\n" );
                return 1;
        }
        return EXIT_SUCCESS;
}

void
rc_usage( void )
{

        printf( "Usage: refresh [OPTION] COMMAND...\n" );
        printf( "Refresh the state of Axigen.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n");
        printf( "  -P port\n");
        printf( "  -t testType\n");

        exit( 0 );
}



static int
rc_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
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
                case 'h':
                        rc_usage(  );
                case 'v':
                        glob.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
