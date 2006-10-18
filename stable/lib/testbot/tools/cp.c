/**
 *   \brief     Wraps around copy operations.
 *   \author    Cristina Balan, Andrei Paduraru
 *   \date      Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "sut.h"
#include "socket.h"
#include <libgen.h>

struct config_s cfg;
static int cp_parseArgs( int argc, char *argv[] );

static int
sendfile( int sock, char *src_file )
{
        int f = open( src_file, O_RDONLY );
        if( f < 0 ) {
                fprintf( stderr, "cp: ERR: Cannot open '%s' : %s\n", src_file,
                         strerror( errno ) );
                return errno;
        }
        char buff[DIM_BUFF];
        int t, w, q;

        while( ( w = read( f, buff, DIM_BUFF ) ) > 0 ) {
                t = 0;
                while( t < w ) {
                        if( ( q = write( sock, buff + t, w - t ) ) < 0 ) {
                                fprintf( stderr,
                                         "cp: ERR: Can't send the file '%s': %s\n",
                                         src_file, strerror( errno ) );
                        }
                        t += q;
                }
        }
        close( f );
        return 0;
}


/*
 * Send a file to socket.
 */
static int
copy_remote( char *host, int port, char *src_file, char *dest_dir )
{
        int cod;
        char cmd[LINE_MAX] = { 0 };
        char *bname, sockfd;

        sockfd = sock_connectTo( host, port );

        int l = util_fileSize( src_file );
        if( l == -1 ) {
                fprintf( stderr, "cp: ERR: '%s' in not a regular file\n",
                         src_file );
                exit( -1 );
        }
        bname = ( char * )basename( src_file );
        sprintf( cmd, "COPY %s %s %d", bname, dest_dir, l );
        sock_sendLine( sockfd, cmd );
        cod = sendfile( sockfd, src_file );
        if( cod ) {
                close( sockfd );
                return 1;
        }

        cod = sock_getStatus( sockfd );

        if( cod < 0 ) {
                fprintf( stderr,
                         "cp: ERR: Receiving command confirmation!\n" );
                exit( -1 );
                return 1;
        }
        return 0;
}



// o sa fie cp sursa destinatie
int
main( int argc, char *argv[] )
{
        // char *type;
        char *type;
        char *str;
        int verbose = 1;

        if( argc < 2 ) {
                printf( "cp: missing file operand\n" );
                printf( "Try `cp -h` for more information.\n" );
                return 1;
        }
        cp_parseArgs( argc, argv );
        str_isEnv( verbose, SUT_TTYPE );
        type = getenv( SUT_TTYPE );

        if( !strcmp( type, "local" ) ) {
                str = ( char * )malloc( strlen( argv[1] ) + strlen( argv[2] ) +
                                        5 );
                sprintf( str, "/bin/cp -R %s %s", argv[1], argv[2] );
                return system( str );
        } else if( !strcmp( type, "remote" ) ) {
                char *host;
                int port;

                str_isEnv( verbose, SUT_HOST );
                str_isEnv( verbose, SUT_PORT );
                host = getenv( SUT_HOST );
                port = atoi( getenv( SUT_PORT ) );
                return copy_remote( host, port, argv[optind], argv[optind+1] );
        } else
                printf( "cp: ERR: Invalid $axi_ttype\n" );
        return 1;
}

void
cp_usage( void )
{

        printf( "Usage: cp [OPTION] COMMAND...\n" );
        printf( "Copy SOURCE to DEST\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n");
        printf( "  -P port\n");
        printf( "  -t testType\n");
        exit( 0 );
}


static int
cp_parseArgs( int argc, char *argv[] )
{
        int c;
        while( ( c = getopt( argc, argv, "t:H:P:hv" ) ) != -1 ) {
                switch ( c ) {
                case 't':
                        if( !strcasecmp( optarg, "remote" )
                        ||( !strcasecmp( optarg, "local" ) ))
                            setenv( "axi_ttype", optarg, 1 );
                        break;
                case 'H':
                        setenv( "axi_host", optarg, 1 );
                        break;
                case 'P':
                        setenv( "axi_port", optarg, 1 );
                        break;
                case 'h':
                        cp_usage(  );
                case 'v':
                        break;
                }
        }
        return TRUE;
}
