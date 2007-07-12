#include "config.h"
#include "core.h"
#include "dbg.h"
#include "strop.h"
#include "sock.h"
#include "sut.h"
#include "xmlrpc.h"
#include "svc_xmlrpc.h"

static int callback_socket( int portno );
static int callback_command( int sckt );
char* clientCallback( const char* filebuf );

int daemon_running=0;



    int
start_xmlrpc(const unsigned int port)
{

    pid_t pid=-1, sid=-1;

    if( (port == 0U) || (port > 65535U) ) {
        dbg_error( "xmlrpc: Cant bind port [%u] : [Illegal value].\n", port );
        return -1;
    }

    /* Daemon */
#if 0
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
        dbg_error( "Can't change to /tmp" );

    core_closeFds();
    /* end Daemon */
#endif
    core_setSigHandlers();
    return callback_socket( port );
}


static int callback_socket( int portno )
{
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int ret, sockfd, opt=1;


    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {   dbg_error( "xmlrpc: opening socket: [%s]\n", strerror( errno ) );
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
    {   dbg_error( "xmlrpc: Can't bind socket: [%s]\n", strerror(errno) );
        return 1;
    }
    ret = listen( sockfd, 5 );
    if( ret < 0 )
    {   dbg_error( "xmlrpc: Can't listen: [%s]\n", strerror(errno) );
        return 1;
    }
    printf( "Running on port:%d\n", portno );
    clilen = sizeof( cli_addr );
    daemon_running=1;
    //closeFds();

    if (-1 == fcntl(sockfd, F_SETOWN, (int) getpid())) return -1;
    while( 0!=daemon_running )
    {   int newsockfd=0;
        newsockfd = accept( sockfd, (struct sockaddr*)&cli_addr, &clilen );
        if( newsockfd < 0 )
        {
            if(errno == EINTR)
            {   continue;
            }else
            {   dbg_error( "accept[%s]\n", strerror(errno) );
                daemon_running=0;
                break;
            }
        }
        //ret = setsockopt( newsockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        //if( ret == -1 ) dbg_error("setsockopt reuseaddr:%s\n", strerror(errno));
        callback_command( newsockfd );
    }
    shutdown( sockfd, 2);
    close( sockfd);
    return 0;
}




static int callback_command( int sckt )
{
#define BUFSZ 102400
    char buf[BUFSZ] = { 0 };
    char *buf1=NULL;
    char *rsp=NULL;
    int n, ret=0;
    char rspBuf[BUFSZ]={0};;
    while( true )
    {   //char *p = NULL;
        memset( buf, 0, BUFSZ );
        n = read( sckt, buf, BUFSZ-1 );

        if( n < 0 )
        {   dbg_error("xmlrpc: Error reading from socket: [%s]\n", strerror(errno) );
            break;
        }
        /* Client closed socket */
        if( n == 0 )
            break;

        buf[( unsigned int )n] = '\0';
        buf1 = strstr(buf, "\r\n\r\n");
        if(buf1)
        {   buf1+=4;
            debug("BODY:[%s]\n", buf1);
            if (-1 == fcntl(sckt, F_SETOWN, (int) getpid())) return -1;
            rsp = clientCallback(buf1);
            if(!rsp) return 0;

            ret = snprintf(rspBuf, BUFSZ, "HTTP/1.1 200 OK\r\nDate: Wed, 07 Mar 2007 09:50:14 GMT\r\nServer: libwww-perl-daemon/1.36\r\nAccept: text/xml\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nRPC-Encoding: XML-RPC\r\nRPC-Server: RPC::XML::Server/1.44\r\n\r\n",
                    strlen(rsp));
            if( ret >= BUFSZ )
                    rspBuf[ BUFSZ-1] = '\0';
            debug("Banner:[%s]\nResponse:[%s]\n", rspBuf, rsp);
            debug("will write on sckt(%d)\n", sckt);
            write(sckt, rspBuf, strlen(rspBuf));
            write(sckt, rsp, strlen(rsp));
            free(rsp);
            rsp=0;
            break;
        }
    } /*while */
    shutdown(sckt,2);
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
    XMLRPC_ServerRegisterMethod( server, "listUserTests", x_listUserTestsCallback );
    XMLRPC_ServerRegisterMethod( server, "runTests", x_runTestsCallback );
    XMLRPC_ServerRegisterMethod( server, "listMachines", x_listMachinesCallback );
    XMLRPC_ServerRegisterMethod( server, "getConfig", x_getConfigCallback );
    XMLRPC_ServerRegisterMethod( server, "setConfig", x_setConfigCallback );
    XMLRPC_ServerRegisterMethod( server, "addMachine", x_addMachineCallback );
    XMLRPC_ServerRegisterMethod( server, "addUserTest", x_addUserTest );
    XMLRPC_ServerRegisterMethod( server, "checkSession", x_checkSessionCallback );
    XMLRPC_ServerRegisterMethod( server, "setSession", x_setSessionCallback );
    XMLRPC_ServerRegisterMethod( server, "registerUser", x_registerUserCallback );
    XMLRPC_ServerRegisterMethod( server, "checkLogin", x_checkLoginCallback );
    XMLRPC_ServerRegisterMethod( server, "checkRemembered", x_checkRememberedCallback );
    XMLRPC_ServerRegisterMethod( server, "listJobs", x_listJobsCallback );
    XMLRPC_ServerRegisterMethod( server, "getErrorLog", x_getErrorLogCallback );
    XMLRPC_ServerRegisterMethod( server, "clearRunnedJobs", x_clearRunnedJobsCallback );
    XMLRPC_ServerRegisterMethod( server, "startManager", x_startManagerCallback );

    request = XMLRPC_REQUEST_FromXML( filebuf, strlen(filebuf), NULL );
    if( !request )
    {    dbg_error("bogus xmlrpc request\n" );
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
