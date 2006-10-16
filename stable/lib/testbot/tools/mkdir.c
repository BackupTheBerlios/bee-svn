/**
 *   \brief    Wraps around mkdir command.
 *   \see      cp.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 **/
#include "util.h"
#include "socket.h"

struct globals_s glob;
static int md_parseArgs( int argc, char *argv[] );

static int
client_mkdir( char *host, int port, char *path )
{
        char command[MAX_LIN] = "";
        int cod = 0, sockfd = -1;

        sockfd = sock_connectTo( host, port );
        sprintf( command, "MKDIR %s", path );
        sock_sendLine( sockfd, command );
        cod = sock_getStatus( sockfd );
        if( cod ) {
                fprintf( stderr,
                         "* mkdir: Error: Could not create the directory %s: %s\n",
                         path, strerror( cod ) );
                close( sockfd );
                return cod;
        }
        close( sockfd );
        return 0;
}

// mkdir has the syntax mkdir path dir_name
int
main( int argc, char *argv[] )
{
        char *tc;               // conexion type
        char *command, *host;
        int port;

        if( argc < 2 )          // trebuie sa am sursa si destinatie
        {
                printf( "mkdir: missing operand\n" );
                printf( "Try `mkdir -h` for more information.\n" );
                return 1;
        }
        md_parseArgs( argc, argv );
        util_isEnv( PUT_TTYPE );
        tc = getenv( PUT_TTYPE );

        if( !strcmp( tc, "local" ) ) {
                command = ( char * )malloc( strlen( argv[1] ) + 8 );
                strcpy( command, "mkdir " );
                strcat( command, argv[1] );
                return system( command );
        } else if( !strcmp( tc, "remote" ) ) {
                util_isEnv( PUT_HOST );
                util_isEnv( PUT_PORT );
                host = getenv( PUT_HOST );
                port = atoi( getenv( PUT_PORT ) );
                return client_mkdir( host, port, argv[1] );
        } else
                printf( "mkdir: Error: Invalid $pt_ttype\n" );
        return 1;
}

void
md_usage( void )
{

        printf( "Usage: mkdir [OPTION] COMMAND...\n" );
        printf( "Create the DIRECTORY, if they do not already exist.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        exit( 0 );
}
static int
md_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" ) )
                                //glob.test_type = TEST_REMOTE;
                                if( !strcasecmp( optarg, "local" ) )
                                        //glob.test_type = TEST_LOCAL;
                                        /*if (!glob.test_type) {
                                           printf("* testbot: Error: Give valid context local/remote.\n");
                                           tb_usage();
                                           } */
                                        setenv( "pt_ttype", optarg, 1 );
                        break;
                case 'H':
                        //glob.hostname = optarg;
                        setenv( "pt_host", optarg, 1 );
                        break;
                case 'P':
                        //glob.port = atoi(optarg);   // fixme
                        setenv( "pt_port", optarg, 1 );
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
