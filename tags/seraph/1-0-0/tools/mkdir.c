/**
 *   \brief    Wraps around mkdir cmd.
 *   \see      cp.c 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include "config.h"
#include "socket.h"
#include "strop.h"
#include "sut.h"

struct config_s cfg;
static int md_parseArgs( int argc, char *argv[] );

static int client_mkdir( char *host, int port, char *path )
{
        char cmd[LINE_MAX] = "";
        int cod = 0, sockfd = -1;

        sockfd = sock_connectTo( host, port );
        sprintf( cmd, "MKDIR %s", path );
        sock_sendLine( sockfd, cmd );
        cod = sock_getStatus( sockfd );
        if( cod ) {
                fprintf( stderr,
                         "! mkdir: Could not create directory [%s]: %s\n",
                         path, strerror( cod ) );
                close( sockfd );
                return cod;
        }
        close( sockfd );
        return 0;
}

int main( int argc, char *argv[] )
{
        char *tc;               // conexion type
        char *host;
        int port;

        if( argc < 2 )          // trebuie sa am sursa si destinatie
        {
                printf( "! mkdir: missing operand\n" );
                printf( "Try `mkdir -h` for more information.\n" );
                return 1;
        }
        md_parseArgs( argc, argv );
        str_isEnv( 1, SUT_TTYPE );
        tc = getenv( SUT_TTYPE );

        if( !strcmp( tc, "local" ) ) {
                char cmd[FILENAME_MAX + 10] = { 0 };    //!fix bof
                sprintf( cmd, "mkdir %s", argv[optind] );
                return system( cmd );
        } else if( !strcmp( tc, "remote" ) ) {
                str_isEnv( 1, SUT_HOST );
                str_isEnv( 1, SUT_PORT );
                host = getenv( SUT_HOST );
                port = atoi( getenv( SUT_PORT ) );
                return client_mkdir( host, port, argv[optind] );
        } else
                printf( "! mkdir: Invalid $axi_ttype\n" );
        return 1;
}

void md_usage( void )
{

        printf( "Usage: mkdir [OPTION] COMMAND...\n" );
        printf( "Create the DIRECTORY, if they do not already exist.\n" );
        printf( "\n" );
        printf
            ( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n" );
        printf( "  -P port\n" );
        printf( "  -t testType\n" );
        exit( 0 );
}
static int md_parseArgs( int argc, char *argv[] )
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
                        md_usage(  );
                case 'v':
                        //glob.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
