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
        printf( "* refresh_client: Removing %s\n", dest );
        sprintf( cmd, "/bin/rm -rf %s", dest );
        if( system( cmd ) ) {
                printf( "* refresh_client: ERR: %s not deleted\n", dest );
                return 1;
        }
        printf( "* refresh_client: copying %s to %s\n", source, dest );
        if( dest[strlen( dest ) - 1] == '/' )
                dest[strlen( dest ) - 1] = '\0';
        sprintf( cmd, "/bin/cp -R %s %s", source, dest );
        if( system( cmd ) ) {
                printf( "* refresh_client: ERR: File not copied\n" );
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



static int
pt_refresh( int test_type, char *source, char *dest, char *host, int port )     // Cristina
{
        struct stat buf;
        int cod = 0;

        cod = stat( source, &buf );
        if( cod != 0 ) {
                fprintf( stderr, "* refresh_client: ERR: %s: %s\n", source,
                         strerror( errno ) );
                exit( -1 );
        }
        if( test_type == TEST_LOCAL ) {
                return refresh_local( source, dest );
        }

        if( test_type == TEST_REMOTE ) {
                if( refresh_remote( host, port, source, dest ) ) {
                        printf( "* refresh_client: ERR: Could not make refresh!!!\n" );
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
        char *path, *host;
        int port;

        rc_parseArgs( argc, argv );
        util_isEnv( PT_TTYPE );
        util_isEnv( PT_STOP );
        util_isEnv( PT_START );
        util_isEnv( PT_DEFDOM );
        util_isEnv( PT_WORKDIR );
        tc = getenv( PT_TTYPE );

        if( !strcasecmp( tc, "local" ) ) {
                printf( "* refresh_client: Working local\n" );
                util_ptStop( TEST_LOCAL, 5, getenv( PT_STOP ) );       //! @todo replace 5 with a proper timeout
                pt_refresh( TEST_LOCAL, getenv( PT_DEFDOM ),
                            getenv( PT_WORKDIR ), 0, 0 );
                util_ptStart( TEST_LOCAL, 5, getenv( PT_START ) );     //! @todo replace 5
        } else if( !strcasecmp( tc, "remote" ) ) {
                printf( "* refresh_client: Working remote\n" );
                util_isEnv( PT_HOST );
                util_isEnv( PT_PORT );

                host = getenv( PT_HOST );
                port = atoi( getenv( PT_PORT ) );
                path = getenv( PT_WORKDIR );
                util_ptStop( TEST_REMOTE, 5, getenv( PT_STOP ) );      //! @todo replace 5
                pt_refresh( TEST_REMOTE, getenv( PT_DEFDOM ),
                            getenv( PT_WORKDIR ), host, port );
                util_ptStart( TEST_REMOTE, 5, getenv( PT_START ) );    //! @todo replace 5
        } else {
                printf( "* refresh_client : Invalid $pt_ttype\n" );
                return 1;
        }
        return EXIT_SUCCESS;
}

void
rc_usage( void )
{

        printf( "Usage: refresh_client [OPTION] COMMAND...\n" );
        printf( "Refresh the state of Axigen.\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        exit( 0 );
}



static int
rc_parseArgs( int argc, char *argv[] )
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
                                           printf("* testbot: ERR: Give valid context local/remote.\n");
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
                        rc_usage(  );
                case 'v':
                        glob.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
