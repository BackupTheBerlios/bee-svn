/**
 *   \brief    Handles the refreshing of ptgen state(users/domains).
 *   \see      ptgenhost.c 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"


struct config_s cfg;
static int rc_parseArgs( int argc, char *argv[] );

#if 0
static int refresh_local( char *source, char *dest )
{
        char cmd[8192] = { 0 };
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

static int refresh_remote( char *host, int port, char *sursa, char *dest )
{
        char command[LINE_MAX] = { 0 };
        int cod, sockfd;

        sockfd = sock_connectTo( host, port );

         /*REFRESH*/ sprintf( command, "REFRESH %s %s", sursa, dest );
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



static int axi_refresh( int test_type, char *source, char *dest, char *host, int port ) // Cristina
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
#endif

int main( int argc, char *argv[] )
{
        char *tc;
        int test_type = TEST_LOCAL;

        rc_parseArgs( argc, argv );
        str_isEnv( 1, SUT_TTYPE );
        str_isEnv( 1, SUT_STOP );
        str_isEnv( 1, SUT_START );
        str_isEnv( 1, SUT_DEFDOM );
        str_isEnv( 1, SUT_WORKDIR );
        tc = getenv( SUT_TTYPE );

        if( !strcasecmp( tc, "local" ) )
                test_type = TEST_LOCAL;
        else if( !strcasecmp( tc, "remote" ) ) {
                str_isEnv( 1, SUT_HOST );
                str_isEnv( 1, SUT_PORT );
                test_type = TEST_REMOTE;
                cfg.hostname = getenv( SUT_HOST );
                cfg.port = atoi( getenv( SUT_PORT ) );
        } else {
                printf( "! refresh : Invalid $axi_ttype\n" );
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
        printf
            ( "  -v, --verbose     print a message for each action executed\n" );
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
                                setenv( "axi_ttype", optarg, 1 );
                                cfg.test_type = TEST_REMOTE;
                        }
                        if( !strcasecmp( optarg, "local" ) ) {
                                setenv( "axi_ttype", optarg, 1 );
                                cfg.test_type = TEST_LOCAL;
                        }
                        break;
                case 'H':
                        setenv( "axi_host", optarg, 1 );
                        cfg.hostname = optarg;
                        break;
                case 'P':
                        setenv( "axi_port", optarg, 1 );
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
