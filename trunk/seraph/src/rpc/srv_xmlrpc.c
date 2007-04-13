#include "config.h"
#include "strop.h"
#include "socket.h"
#include "sut.h"
#include "xmlrpc.h"
#include "svc_xmlrpc.h"
#include <signal.h>

static int callback_socket( int portno );
static int callback_command( int sckt );
char* clientCallback( char* filebuf );

int running = 1;

    int
start_xmlrpc(const unsigned int port)
{

    pid_t pid=-1, sid=-1;

    if( port == 0 || port > 65535 ) {
        debug( "Cant bind port: %d : Illegal value.\n", port );
        return -1;
    }

    /* Daemon */
    pid = fork( );

    if( pid < 0 )
    {   fprintf( stderr, "rsh: Error while forking\n" );
        exit( EXIT_FAILURE );
    }

    /* Parent */
    if( pid > 0 )
    {   exit( EXIT_SUCCESS );
    }

    /* Child code */
    ( void )umask( 0 );
    sid = setsid(  );
    if( sid < 0 )
        exit( EXIT_FAILURE );

    if( ( chdir( "/home/groleo" ) ) < 0 )
    {   perror( "rsh: Can't change to /tmp" );
        exit( EXIT_FAILURE );
    }
    if( signal( SIGINT, sut_sigint ) == SIG_ERR )
    {   perror( "signal() error!" );
        return 1;
    }

    if( signal( SIGPIPE, sut_sigpipe ) == SIG_ERR )
    {   perror( "signal() error!" );
        return 1;
    }

    /* Daemon */
    callback_socket( port );
    return 0;
}


static int callback_socket( int portno )
{
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int cod, sockfd;


    printf( "Running on port:%d\n", portno );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {  fprintf( stderr, "rsh: ERR: opening socket: %s\n",
                strerror( errno ) );
        return 1;
    }
    bzero( ( char * )&serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    if( bind( sockfd, ( struct sockaddr * )&serv_addr, sizeof( serv_addr ) )
            < 0 )
    {   debug( "rsh: ERR on binding: %s", strerror(errno) );
        exit( -1 );
    }
    cod = listen( sockfd, 5 );
    if( cod < 0 )
    {   debug( "rsh: ERR on listen %s", strerror(errno) );
        exit( -1 );
    }
    clilen = sizeof( cli_addr );

    extern int running ;
    while( running )
    {   int newsockfd;
        newsockfd =
            accept( sockfd, ( struct sockaddr * )&cli_addr, &clilen );
        if( newsockfd < 0 )
        {   perror( "rsh: ERR on accept" );
            exit( -1 );
        }
        callback_command( newsockfd );
    }
    close( sockfd);
    return 0;
}




static int callback_command( int sckt )
{
    char buf[8192] = { 0 };
    char*buf1=0,*rsp;
    int ok = 1;
    int n;//, i;
    char banner[8192]={0};;
    while( ok ) {
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
            debug("%s\n", buf1);
            rsp = clientCallback(buf1);
            sprintf(banner,"HTTP/1.1 200 OK\r\nDate: Wed, 07 Mar 2007 09:50:14 GMT\r\nServer: libwww-perl-daemon/1.36\r\nAccept: text/xml\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nRPC-Encoding: XML-RPC\r\nRPC-Server: RPC::XML::Server/1.44\r\n\r\n",
                    strlen(rsp));
            debug("\n[%s]\n[%s]\n", banner, rsp);
            write(sckt, banner, strlen(banner));
            write(sckt, rsp, strlen(rsp));
            free(rsp);
            ok=false;
        }
    } /*while */
    close( sckt );
    return 0;
}
/* with the exception of the registration calls, most everything in main
 * only needs to be written once per server.
 */
char* clientCallback( char* filebuf )
{
    XMLRPC_SERVER server;
    XMLRPC_REQUEST request, response;

    /* create a new server object */
    server = XMLRPC_ServerCreate(  );

    /* Register public methods with the server */
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

    /* Now, let's get the client's request from stdin....
     * This will be read from a  socket
     */
    {
        /*         char filebuf[4096];     // not that intelligent.  sue me.
                   int len =
                   fread( filebuf, sizeof( char ), sizeof( filebuf ) - 1,

                   if( len ) {
                   filebuf[len] = 0;
                   stdin );
                   */
        // parse the xml into a request structure
        request =
            XMLRPC_REQUEST_FromXML( ( const char * )filebuf,
                    strlen(filebuf), NULL );
        //         }
    }
    if( !request ) {
        fprintf( stderr, "bogus xmlrpc request\n" );
        return 0;
    }
    /*
     *  The interesting part is below
     */

    /* create a response struct */
    response = XMLRPC_RequestNew( );
    XMLRPC_RequestSetRequestType( response, xmlrpc_request_response );

    /* call server method with client request and assign the response to our response struct */
    XMLRPC_RequestSetData( response,
            XMLRPC_ServerCallMethod( server, request,
                NULL ) );

    /* be courteous. reply in same vocabulary/manner as the request. */
    XMLRPC_RequestSetOutputOptions( response,
            XMLRPC_RequestGetOutputOptions
            ( request ) );

    /* serialize server response as XML */
    char *outBuf = XMLRPC_REQUEST_ToXML( response, 0 );

    if( outBuf ) {
        printf( outBuf );
    }
    // cleanup.  null safe.
    XMLRPC_RequestFree( request, 1 );
    XMLRPC_RequestFree( response, 1 );
    XMLRPC_ServerDestroy( server );

    return outBuf;
}
