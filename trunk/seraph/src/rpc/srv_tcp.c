/**
 *   \brief    Remote shell.
 *   \see      rsh.c
 *   \author   
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"
#include "rshd.h"
#include "svc_tcp.h"

Callbacks callbacks[] = {
        {"COPY", t_copyCallback}
        ,
        {"RM", t_rmCallback}
        ,
        {"MKDIR", t_mkdirCallback}
        ,
        {"EXECUTE", t_executeCallback}
        ,
        {"CHECKCORE", t_checkCoreCallback}
        ,
        {"START", t_startCallback}
        ,
        {"STOP", t_stopCallback}
        ,
        {"REFRESH", t_refreshCallback}
        ,
        {NULL, NULL}
};

static int callback_socket( short unsigned int portno );
static int callback_command( int sckt );

int start_rawrpc( const unsigned int port )
{

        pid_t pid, sid;
        if( (port == 0U) || (port > 65535U) ) {
                fprintf( stderr, "Cant bind port: %d\n", port );
                return -1;
        }
        /* Daemon */
        pid = fork(  );
        if( pid < 0 ) {
                fprintf( stderr, "rsh: Error while forking\n" );
                exit( EXIT_FAILURE );
        }
        if( pid > 0 ) {         /* Parent */
                exit( EXIT_SUCCESS );
        }
        /* Child code */
        ( void )umask( 0 );
        sid = setsid(  );
        if( sid < 0 )
        {       exit( EXIT_FAILURE );}
        if( ( chdir( "/" ) ) < 0 )
        {       perror( "rsh: Can't change to /" );
                exit( EXIT_FAILURE );
        }
        if( signal( SIGINT, sut_sigint ) == SIG_ERR )
        {       perror( "signal() error!" );
                return 1;
        }

        if( signal( SIGPIPE, sut_sigpipe ) == SIG_ERR )
        {       perror( "signal() error!" );
                return 1;
        }
#if 0
        printf( "Closing fd's\n" );
        fclose( stdin );
        fclose( stdout );
        fclose( stderr );
        stdout = fopen( "/tmp/seraph.stdout", "a" );
        stderr = fopen( "/tmp/seraph.stderr", "w" );
#endif
        /* Daemon */
        callback_socket( port );
        return 0;
}


static int callback_socket( const unsigned short int portno )
{
        unsigned int clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int cod, sockfd;


        printf( "Running on port:%d\n", portno );
        sockfd = socket( AF_INET, SOCK_STREAM, 0 );
        if( sockfd < 0 ) {
                fprintf( stderr, "rsh: ERR: opening socket: %s\n",
                         strerror( errno ) );
                return 1;
        }
        memset( ( char * )&serv_addr, '\0', sizeof( serv_addr ) );
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons( portno );
        if( bind( sockfd, ( struct sockaddr * )&serv_addr, sizeof( serv_addr ) )
            < 0 ) {
                perror( "rsh: ERR on binding" );
                exit( -1 );
        }
        cod = listen( sockfd, 5 );
        if( cod < 0 ) {
                perror( "rsh: ERR on listen" );
                exit( -1 );
        }
        clilen = sizeof( cli_addr );

        while( 1 ) {
                int newsockfd;
                newsockfd =
                    accept( sockfd, ( struct sockaddr * )&cli_addr, &clilen );
                if( newsockfd < 0 ) {
                        perror( "rsh: ERR on accept" );
                        exit( -1 );
                }
                callback_command( newsockfd );
        }
}




static int callback_command( int sckt )
{
        char buf[8192] = { '\0' };
        int ok = 1;
        int n, i;

        while( ok ) {
                char *p = NULL;
                memset( buf, 0, 8192 );
                n = read( sckt, buf, 8191 );

                if( n < 0 ) {
                        fprintf( stderr,
                                 "! daemon: Error reading from socket\n" );
                        break;
                }
                /* Client closed socket */
                if( n == 0 ) {
                        close( sckt );
                        break;
                }
                buf[( unsigned int )n] = '\0';

                /* extract keyword */
                p = strchr( buf, ' ' );
                if( p ) {
                        *p++ = '\0';
                        for( i = 0; *( p + i ) >= 32 && *( p + i ) != '\0';
                             ++i );
                        *( p + i ) = '\0';
                        printf( "cmd[%s] param[%s]\n", buf, p );
                } else {
                        printf( "\n--Unknown command--\n" );
                        continue;
                }
                for( i = 0; callbacks[i].keyword != NULL; i++ )
                        if( str_matches( buf, callbacks[i].keyword ) ) {
                                callbacks[i].call( sckt, p );
                                break;
                        }
        }                       /*while */
        close( sckt );
        return 0;
}
