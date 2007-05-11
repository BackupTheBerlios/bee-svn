/**
 *   \brief    Remote shell.
 *   \see      rsh.c
 *   \author   
 *   \date     Thu Aug 17 17:38:13 2006
 *
 */
#include "config.h"
#include "core.h"
#include "dbg.h"
#include "strop.h"
#include "sock.h"
#include "sut.h"
#include "svc_tcp.h"

extern int daemon_running;
extern struct config_s cfg;

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

    pid_t pid=-1, sid=-1;
    if( (port == 0U) || (port > 65535U) )
    {   dbg_error( "rawrpc: Cant bind port [%u]: [Illegal value].\n", port );
        return -1;
    }
#if 0
    /* Daemon */
    pid = fork( );

    if( pid < 0 )
    {   debug( "Unable to fork(): %s\n", strerror(errno) );
        return EXIT_FAILURE ;
    }

    /* Parent */
    if( pid > 0 )
        exit( EXIT_SUCCESS );

    /* Child code */
    ( void )umask( 0 );
    sid = setsid(  );
    if( sid < 0 )
        exit( EXIT_FAILURE );

    if( ( chdir( "/tmp" ) ) < 0 )
        debug( "Can't change to /tmp" );
    /* Daemon */
    long fdlimit = -1, i;
    int fderr, fdout,fdin;

    fdlimit = sysconf (_SC_OPEN_MAX);
    if (fdlimit == -1) {
        fdlimit = 3;
        debug("cant fdlimit\n");
    }
    fdlimit = 3;
    for (i = 0; i < fdlimit; i++)
        close (i);

    fderr = open ( "/home/groleo/ERRORS", O_RDWR, 0);/*TODO*/
    fdout = open ( "/home/groleo/OUT", O_RDWR, 0);/*TODO*/
    fdin  = open ( "/dev/null", O_RDWR,0);

    if (fdin != -1 || fdout != -1 || fderr != -1)
    {
        dup2 (fdin, STDIN_FILENO);
        dup2 (fdout, STDOUT_FILENO);
        dup2 (fderr, STDERR_FILENO);
        if (fdin > 2 )
            close (fdin);
        if (fdout > 2 )
            close (fdout);
        if (fderr > 2 )
            close (fderr);
    }
#endif
    struct sigaction action;
    action.sa_handler = core_onSigInt;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction( SIGINT, &action, NULL);
    sigaction( SIGPIPE, &action, NULL);

    return callback_socket( port );
}


static int callback_socket( const unsigned short int portno )
{
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int ret, sockfd, opt=1;


    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {   dbg_error("rawrpc: opening socket: [%s]\n", strerror( errno ) );
        return 1;
    }

    memset( ( char * )&serv_addr, '\0', sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );

    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if( ret == -1 ) dbg_error("setsockopt reuseaddr: [%s]\n", strerror(errno));

    if( bind( sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr) )
            < 0 )
    {   dbg_error( "rawrpc: Can't bind socket: [%s]\n", strerror(errno) );
        return 1;
    }
    ret = listen( sockfd, 5 );
    if( ret < 0 )
    {   dbg_error( "rawrpc: Can't listen : [%s]\n", strerror(errno) );
        return 1;
    }
    printf( "Running on port:%d\n", portno );
    clilen = sizeof( cli_addr );
    daemon_running=1;

    while( daemon_running!=0 )
    {   int newsockfd;
        newsockfd = accept( sockfd, (struct sockaddr*)&cli_addr, &clilen );

        if( newsockfd < 0 )
        {   dbg_error( "rawrpc: accept[%s]\n", strerror(errno) );
            daemon_running = 0;
            break;
        }
        callback_command( newsockfd );
    }
    shutdown(sockfd,2);
    close(sockfd);
    return 0;
}




static int callback_command( int sckt )
{
    char buf[8192] = { '\0' };
    int ok = 1;
    int n, i;

    while( ok )
    {   char *p = NULL;
        memset( buf, 0, 8192 );
        n = read( sckt, buf, 8191 );

        if( n < 0 )
        {   dbg_error("rawrpc: Error reading from socket : [%s]\n", strerror(errno) );
            break;
        }
        /* Client closed socket */
        if( n == 0 )
            break;

        buf[( unsigned int )n] = '\0';

        /* extract keyword */
        p = strchr( buf, ' ' );
        if( p )
        {   *p++ = '\0';
            for( i = 0; *( p + i ) >= 32 && *( p + i ) != '\0';
                    ++i );
            *( p + i ) = '\0';
            dbg_verbose( "cmd[%s] param[%s]\n", buf, p );
        } else
        {   printf( "\n--Unknown command--\n" );
            continue;
        }
        for( i = 0; callbacks[i].keyword != NULL; i++ )
            if( str_matches( buf, callbacks[i].keyword ) ) {
                callbacks[i].call( sckt, p );
                break;
            }
    }/*while */
    shutdown( sckt, 2);
    close( sckt );
    return 0;
}
