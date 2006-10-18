/**
 *   \brief    Wraps around rm command.
 *   \see      ptgenhost.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */

#include "util.h"
#include "socket.h"

struct globals_s glob;
static int rm_parseArgs( int argc, char *argv[] );

int
main( int argc, char *argv[] )
{
        char *tc;               // conexion type
        char *host;
        int port;

        if( argc < 2 )          // trebuie sa am sursa si destinatie
        {
                printf( "rm: missing file operand\n" );
                printf( "Try `rm -h` for more information.\n" );
                return 1;
        }
        rm_parseArgs( argc, argv );
        util_isEnv( AXI_TTYPE );
        tc = getenv( AXI_TTYPE );
        if( argc < 2 ) {
                fprintf( stderr, "ERR: no valid sintax" );
                return 1;
        }

        if( !strcmp( tc, "local" ) ) {
                char cmd[PATH_MAX+10] = { 0 } ;
                sprintf(cmd, "/bin/rm -rf %s", argv[optind] ) ;
                return system( cmd );
        } else if( !strcmp( tc, "remote" ) ) {
                util_isEnv( AXI_HOST );
                util_isEnv( AXI_PORT );
                host = getenv( AXI_HOST );
                port = atoi( getenv( AXI_PORT ) );
                return util_rmRemote( host, port, argv[optind] );

        } else
                printf( "Invalid $axi_ttype\n" );
        return 1;
}



void
rm_usage( void )
{

        printf( "Usage: rm [OPTION] FILE...\n" );
        printf( "Remove (unlink) the FILE(s).\n" );
        printf( "\n" );
        printf( "  -v, --verbose     print a message for each action executed\n" );
        printf( "  -h, --help        display this help and exit\n" );
        printf( "  -H hostname\n");
        printf( "  -P port\n");
        printf( "  -t testType\n");
        exit( 0 );
}



static int
rm_parseArgs( int argc, char *argv[] )
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
                        rm_usage(  );
                case 'v':
                        glob.verbose = TRUE;
                        break;
                }
        }
        return TRUE;
}
