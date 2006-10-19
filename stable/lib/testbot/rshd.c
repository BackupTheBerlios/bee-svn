/**
 *   \brief    Remote shell.
 *   \see      rsh.c
 *   \author   Cristina Balan, Andrei Paduraru, Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "strop.h"
#include "socket.h"
#include "sut.h"
#include "rshd.h"
#include "callbacks.h"

Callbacks callbacks[] = {
        {"COPY", callback_copy},
        {"RM", callback_rm},
        {"MKDIR", callback_mkdir},
        {"EXECUTE", callback_execute},
        {"CHECKCORE", callback_checkCore},
        {"START", callback_start},
        {"STOP", callback_stop},
        {"REFRESH", callback_refresh},
        {NULL, NULL}
};

static int callback_socket(int portno)  ;
static int callback_command(int socket );

int
rsh_main( int port )
{

        pid_t pid, sid;
        if( port <= 0 || port > 65535 ) {
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
        umask( 0 );
        sid = setsid(  );
        if( sid < 0 )
                exit( EXIT_FAILURE );
        if( ( chdir( "/" ) ) < 0 ) {
                perror( "rsh: Can't change to /" );
                exit( EXIT_FAILURE );
        }
        if( signal( SIGINT, sut_sigint ) == SIG_ERR ) {
                perror( "signal() error!" );
                return 1;
        }

        if( signal( SIGPIPE, sut_sigpipe ) == SIG_ERR ) {
                perror( "signal() error!" );
                return 1;
        }
        printf( "Closing fd's\n" );
#if 0
        close( STDIN_FILENO );
        close( STDOUT_FILENO );
        close( STDERR_FILENO );
        stdout = fopen( "/dev/null", "w" );
        stderr = fopen( "/dev/null", "w" );
#endif
        /* Daemon */
        callback_socket( port );
        return 0;
}


static int
callback_socket( int portno )
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
        bzero( ( char * )&serv_addr, sizeof( serv_addr ) );
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons( portno );
        if( bind( sockfd, ( struct sockaddr * )&serv_addr, sizeof( serv_addr ) )
            < 0 ) {
                perror("rsh: ERR on binding");
                exit( -1 );
        }
        cod = listen( sockfd, 5 );
        if( cod < 0 ) {
                perror( "rsh: ERR on listen");
                exit( -1 );
        }
        clilen = sizeof( cli_addr );

        while( 1 ) {
                int newsockfd ;
                newsockfd =
                        accept( sockfd, ( struct sockaddr * )&cli_addr,
                                &clilen );
                if( newsockfd < 0 ) {
                        perror( "rsh: ERR on accept");
                        exit( -1 );
                }
                callback_command( newsockfd );
        }
}




static int
callback_command( int socket )
{
        char buf[8192] = { 0 };
        int ok = 1;
        int n, i;

        while( ok ) {
                char *p = NULL;
                memset( buf, 0, 8192 );
                n = read( socket, buf, 8191 );

                if( n < 0 ) {
                        fprintf( stderr, "ERROR reading from socket\n" );
                        ok = 0;
                }
                /* Client closed socket */
                if( n == 0 ) {
                        ok = 0;
                        close( socket );
                        break;
                }
                buf[n] = '\0';

                /* extract keyword */
                p = strchr( buf, ' ' );
                if( p ) {
                        *p++ = '\0';
                        for( i = 0; *(p+i) >= 32 && *(p+i) != '\0'; ++i );
                        *(p+i) = '\0';
                        printf( "cmd[%s] param[%s]\n", buf, p );
                } else {
                        printf( "\n--Unknown command--\n" );
                        continue;
                }
                for( i = 0; callbacks[i].keyword != NULL; i++ )
                        if( str_matches( buf, callbacks[i].keyword ) ) {
                                callbacks[i].call( socket, p );
                                break;
                        }
        }/*while*/
        close( socket ) ;
        return 0;
}

