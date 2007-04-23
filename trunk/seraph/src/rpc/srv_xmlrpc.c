#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"
#include "xmlrpc.h"
#include "svc_xmlrpc.h"
#include <limits.h>

static int callback_socket( int portno );
static int callback_command( int sckt );
char* clientCallback( const char* filebuf );

int running = 1;

    int
start_xmlrpc(const unsigned int port)
{

    pid_t pid=-1, sid=-1;

    if( (port == 0U) || (port > 65535U) ) {
        debug( "Cant bind port: %u : Illegal value.\n", port );
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
    if( signal( SIGINT, sut_sigint ) == SIG_ERR )
    {   debug( "1:signal() error!\n" );
        return 1;
    }

    if( signal( SIGPIPE, sut_sigpipe ) == SIG_ERR )
    {   debug( "3:signal() error!\n" );
        return 1;
    }

    return callback_socket( port );
}


static int callback_socket( int portno )
{
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int cod, sockfd;


    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {  debug( "ERR: opening socket: %s\n", strerror( errno ) );
        return 1;
    }
    memset( ( char * )&serv_addr, '\0', sizeof( serv_addr ) );

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    if( bind( sockfd, ( struct sockaddr * )&serv_addr, sizeof( serv_addr ) )
            < 0 )
    {   debug( "ERR on binding: %s\n", strerror(errno) );
        return 1;
    }
    cod = listen( sockfd, 5 );
    if( cod < 0 )
    {   debug( "ERR on listen %s\n", strerror(errno) );
        return 1;
    }
    printf( "Running on port:%d\n", portno );
    clilen = sizeof( cli_addr );

    extern int running ;
    if (-1 == fcntl(sockfd, F_SETOWN, (int) getpid())) return -1;
    while( running )
    {   int newsockfd=0;
        newsockfd = accept( sockfd, ( struct sockaddr * )&cli_addr, &clilen );
        if( newsockfd < 0 )
        {   debug( "rsh: ERR on accept\n" );
            running=0;
            break;
        }
        callback_command( newsockfd );
    }
    close( sockfd);
    return 0;
}




static int callback_command( int sckt )
{
    char buf[8192] = { 0 };
    char *buf1=NULL;
    char *rsp=NULL;
    int n;//, i;
    char banner[8192]={0};;
    while( true ) {
        //char *p = NULL;
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
        buf1 = strstr(buf, "\r\n\r\n");
        if(buf1){
            buf1+=4;
            debug("BODY:[%s]\n", buf1);
            if (-1 == fcntl(sckt, F_SETOWN, (int) getpid())) return -1;
            rsp = clientCallback(buf1);
            if(!rsp) return 0;

            sprintf(banner,"HTTP/1.1 200 OK\r\nDate: Wed, 07 Mar 2007 09:50:14 GMT\r\nServer: libwww-perl-daemon/1.36\r\nAccept: text/xml\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nRPC-Encoding: XML-RPC\r\nRPC-Server: RPC::XML::Server/1.44\r\n\r\n",
                    strlen(rsp));
            debug("Banner:[%s]\nResponse:[%s]\n", banner, rsp);
            debug("will write on sckt(%d)\n", sckt);
            write(sckt, banner, strlen(banner));
            write(sckt, rsp, strlen(rsp));
            free(rsp);
            rsp=0;
            shutdown(sckt,2);
            close(sckt);
            return 0;
        }
    } /*while */
    close(sckt);
    return 0;
}
/* with the exception of the registration calls, most everything in main
 * only needs to be written once per server.
 */
char* clientCallback( const char* filebuf )
{
    XMLRPC_SERVER server;
    XMLRPC_REQUEST request, response;

    /* create a new server object */
    server = XMLRPC_ServerCreate(  );

    /* Register public methods with the server */
    /* TODO: place these in a header file( and some macro magic ) */
    XMLRPC_ServerRegisterMethod( server, "start", x_startCallback );
    XMLRPC_ServerRegisterMethod( server, "stop", x_stopCallback );
    XMLRPC_ServerRegisterMethod( server, "rm", x_rmCallback );
    XMLRPC_ServerRegisterMethod( server, "mkdir", x_mkdirCallback );
    XMLRPC_ServerRegisterMethod( server, "execute", x_executeCallback );
    XMLRPC_ServerRegisterMethod( server, "checkcore", x_checkCoreCallback );
    XMLRPC_ServerRegisterMethod( server, "listTests", x_listTestsCallback );
    XMLRPC_ServerRegisterMethod( server, "runTests", x_runTestsCallback );
    XMLRPC_ServerRegisterMethod( server, "listMachines", x_listMachinesCallback );
    XMLRPC_ServerRegisterMethod( server, "getConfig", x_getConfigCallback );
    XMLRPC_ServerRegisterMethod( server, "setConfig", x_setConfigCallback );
    XMLRPC_ServerRegisterMethod( server, "addMachine", x_addMachineCallback );
    XMLRPC_ServerRegisterMethod( server, "checkSession", x_checkSessionCallback );
    XMLRPC_ServerRegisterMethod( server, "setSession", x_setSessionCallback );
    XMLRPC_ServerRegisterMethod( server, "checkLogin", x_checkLoginCallback );
    XMLRPC_ServerRegisterMethod( server, "checkRemembered", x_checkRememberedCallback );
    XMLRPC_ServerRegisterMethod( server, "listJobs", x_listJobsCallback );

    request = XMLRPC_REQUEST_FromXML( filebuf, strlen(filebuf), NULL );
    if( !request ) {
        fprintf( stderr, "bogus xmlrpc request\n" );
        return NULL;
    }

    /* create a response struct */
    response = XMLRPC_RequestNew( );
    XMLRPC_RequestSetRequestType( response, xmlrpc_request_response );

    /* call server method with client request and assign the response to our response struct */
    XMLRPC_RequestSetData( response, XMLRPC_ServerCallMethod( server, request, NULL ) );
    if( !response ) return NULL;

    /* be courteous. reply in same vocabulary/manner as the request. */
    XMLRPC_RequestSetOutputOptions( response, XMLRPC_RequestGetOutputOptions ( request ) );

    /* serialize server response as XML */
    char *outBuf = XMLRPC_REQUEST_ToXML( response, 0 );

    if( outBuf ) {
        debug( "%s\n", outBuf );
    }
    // cleanup.  null safe.
    XMLRPC_RequestFree( request, 1 );
    XMLRPC_RequestFree( response, 1 );
    XMLRPC_ServerDestroy( server );

    return outBuf;
}
