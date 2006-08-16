#include "Socket.h"
#if defined DEBUG
    #include "Debug.h"
#endif


#ifdef WIN32
    #include <winsock.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std ;

void
Socket::latency(bool modem)
{
    if(modem)
        latency_ = 999990 ;
    else
        latency_ = 0 ;
}

/**
 * Default constructor. **/
Socket::Socket() : sock_(-1), tout_(5), usec_tout_(0), is_open_(false), is_conn_(false), sentBytes(0), latency_(0)
, is_sync_(true)
{
    //debug( "C" ) ;
    #ifdef WIN32
    WSADATA wsaData ;
    WORD wVersionRequested ;
    int err ;

    wVersionRequested = MAKEWORD( 2, 0 ) ; // 2.0 and above version of WinSock
    err = WSAStartup( wVersionRequested, &wsaData ) ;
    if ( err != 0 )
    {   throw Socket::Exception("Couldn't not find a usable WinSock DLL") ;
        exit(-1) ;      // Replace with your own exit code
    }
    #endif

}//*Socket::Socket



/**
 * Default destructor. **/
Socket::~Socket()
{
    //debug("D") ;
    #ifdef WIN32
    WSACleanup() ;
    #endif
}//*Socket::~Socket




/**
 * Create a socket file descriptor.
 * @param[in] family_i Socket family.
 * @param[in] type_i   Socket type.
 * @param[in] protocol_i Socket protocol.**/

    void
Socket::open( Socket::Family::Flag family, Socket::Type::Flag type, const int protocol_i)
{

    //debug("B" ) ;
    int rval = socket( family, type, protocol_i ) ;

    //debug( "sock=%i", rval ) ;
    if( rval == -1 ) throw Socket::Exception( "unable to create socket", strerror(errno) ) ;


    sock_   = rval ;
    family_ = family ;
    type_   = type ;
    is_open_= true ;

    /* Set to non-blocking */
    int oldopts, rc ;
    oldopts = fcntl( sock_, F_GETFL, 0);
    rc = fcntl( sock_, F_SETFL, oldopts | O_NONBLOCK);
    if( rc == -1 ) throw Socket::Exception( "unable to non-block", strerror(errno) );

    //debug("E");
}//*Socket::open



/**
 * Connect the socket to a remote peer.
 * @todo connect should be non-blocking, and wait 5 seconds
 * @param[in] hostNamE The name of host.
 * @param p Remote peer port.**/
    void
Socket::connect( const char* h, const int p)
{
    char* hostName = (char*)h ;
    //debug( "host=%s, port=%i", hostName , p ) ;

    if( !is_open() ) throw Socket::Exception( "Invalid socket handler." ) ;
    sockaddr_in     dest ;  // man 7 ip
    struct hostent* host ;

    dest.sin_port   = htons( p ) ;
    dest.sin_family = family_ ;

    host = gethostbyname( hostName ) ;//not thread safe?
    if( !host ) throw Socket::Exception( "unable to resolve hostname", strerror(errno) ) ;

    memcpy( (char*)&dest.sin_addr, (char*)host->h_addr, host->h_length ) ;

    fd_set rfds, wfds ;
    struct timeval tv ;
    tv.tv_sec = tout_ ;
    tv.tv_usec = usec_tout_ ;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET( sock_,&rfds) ;
    FD_SET( sock_,&wfds) ;
    int rc = ::connect( sock_, (sockaddr*)&dest, sizeof(dest) ) ;
    if( rc == -1)
    {
        /* Connection in progress, select could be used */
        if( errno == EINPROGRESS )
        {
            int sc = select( sock_+1, &rfds, &wfds, 0, &tv ) ;
            if( sc == -1)
                throw Socket::Exception( "select failed",strerror(errno) )  ;   // error in select()
            else if( sc )
            {
                debug("data is available now.\n");             // connected
                is_conn_ = true ;
                return ;
            }
            else
            {
                debug("connection timed out");   
                throw Socket::Exception( "connection timed out" ) ;    // Connect timeout( TODO: ++errCnt
            }
        }else
        {
            debug("connect failed");
            throw Socket::Exception( "connect failed", strerror(errno)) ;        // Connect failed( TODO: ++errCnt
        }
    }
    is_conn_ = true ;
}//*Socket::connect



/**
 * dest is resolved outside of the Socket class,
 * doing so might gain some speed. */
void
Socket::connect( sockaddr_in* dest )
{
    debug("Connecting..."); // TODO
    fd_set rfds, wfds ;
    struct timeval tv ;
   
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET( sock_,&rfds) ;
    FD_SET( sock_,&wfds) ;

    tv.tv_sec  = tout_ ;      // seconds to sleep.(SpecMAIL says 5 seconds is QoS required)
    tv.tv_usec = usec_tout_ ;          // micro seconds to sleep.
    
    int rc = ::connect( sock_+1, (sockaddr*)&dest, sizeof(dest) ) ;
    if( rc == -1)
    {
        /* Connection in progress, select could be used */
        if( errno == EINPROGRESS )
        {
            int sc = select( 1, &rfds, &wfds, 0, &tv ) ;
            if( sc == -1)
                throw Socket::Exception( "error in select", strerror(errno) )  ;   // error in select()
            else if( sc )
            {
                debug("Data is available now.");             // connected
                is_conn_ = true ;
                return ;
            }
            else
            {
                debug("connect timeout");
                throw Socket::Exception( "connect timeout",strerror(errno) ) ;    // Connect timeout( TODO: increment errCnt
            }
        }else
        {
            debug("connect failed");
            throw Socket::Exception( "connect failed", strerror(errno) ) ;        // Connect failed( TODO: ++errCnt
        }
    }
    is_conn_ = true ;
}

/**
 * Send a string on socket.
 * @param[in] msg  Message to send on socket.
 * @param[in] size Message size **/
    void
Socket::write( const char* msg, const int& size )
{
    //debug("msg=%s", msg.c_str() ) ;
    int rval ;

    if( !size ) throw Socket::Exception( "empty message" ) ;
    
    timeval wait ;      //! timeout for select
    fd_set  wd, er ;    //! readable and error file descriptors set
    FD_ZERO( &wd ) ;
    FD_ZERO( &er ) ;
    FD_SET ( sock_, &wd ) ;
    FD_SET ( sock_, &er ) ;

    wait.tv_sec  = tout_ ;      // seconds to sleep.(SpecMAIL says 5 seconds is QoS required)
    wait.tv_usec = usec_tout_ ;          // micro seconds to sleep.
    
    rval = select( sock_ +1, NULL, &wd, &er, &wait ) ;

    if( rval == -1 ) throw Socket::Exception( "select readfailed",strerror( h_errno ) );
    if( rval  )
    {
        rval = ::send( sock_, msg, size, 0 ) ;
        if( rval == -1 ) throw Socket::Exception( "write failed:", strerror(errno) ) ;
    }else
    {
        throw Socket::Exception( "write timed out" ) ;
    }

    sentBytes+=size ;
    if( sentBytes >= 6000 ) //bytesSent
    {
        usleep(latency_) ;
        sentBytes = 0 ;
    }
}//*Socket::send



/** Send a string on socket. **/
void
Socket::write( const string& m )
{
    //debug("msg=%s", msg.c_str() ) ;
    int rval ;

    if( m.empty() ) throw Socket::Exception( "empty message" ) ;
    timeval wait ;      //! timeout for select
    fd_set  wd, er ;    //! readable and error file descriptors set
    FD_ZERO( &wd ) ;
    FD_ZERO( &er ) ;
    FD_SET ( sock_, &wd ) ;
    FD_SET ( sock_, &er ) ;

    wait.tv_sec  = tout_ ;      // seconds to sleep.(SpecMAIL says 5 seconds is QoS required)
    wait.tv_usec = usec_tout_ ;          // micro seconds to sleep.

    rval = select( sock_ +1, NULL, &wd, &er, &wait ) ;

    if( rval == -1 ) throw Socket::Exception( "select readfailed",strerror( h_errno ) );
    if( rval  )
    {
        rval = ::send( sock_, m.c_str(), m.size(), 0 ) ;
        if( rval == -1 ) throw Socket::Exception( "write failed:", strerror(errno) ) ;
    }else
    {
        throw Socket::Exception( "write timed out" ) ;
    }
    sentBytes+=m.size() ;
    if( sentBytes >= 6000 ) //bytesSent
    {
        usleep(latency_) ;
        sentBytes = 0 ;
    }
}//* Socket::write



/** Receives a string from socket. **/
    void
Socket::read( char* s, unsigned int n )
{
    timeval wait ;      //! timeout for select
    fd_set  rd, er ;    //! readable and error file descriptors set
    //char    recvb[ 1024+1] ; //! receive buffer

    resp_.clear() ;

    FD_ZERO( &rd ) ;
    FD_ZERO( &er ) ;
    FD_SET ( sock_, &rd ) ;
    FD_SET ( sock_, &er ) ;

    wait.tv_sec  = tout_ ;      // seconds to sleep.(SpecMAIL says 5 seconds is QoS required)
    wait.tv_usec = usec_tout_ ;          // micro seconds to sleep.

    int rval = select( sock_ +1, &rd, NULL, &er, &wait ) ;

    if( rval == -1 ) throw Socket::Exception( "select readfailed",strerror( h_errno ) );
    if( rval  )
    {
        int rc = ::read( sock_, s, n ) ;
        if(-1 == rc ) { debug("read error:%s", strerror(errno)); throw Socket::Exception( "Read erorr:", strerror(errno) ) ; }
        if( 0 == rc ) { debug("connection reset by server"); throw Socket::Exception( "Connection reset by peer" ) ; }
        s[rc] = '\0' ; //is this necessary ?
        resp_ += s ;
        //debug("resp_=%s", resp_.c_str() ) ;
        return ;
    }else
    {
        debug("read timed out");
        throw Socket::Exception( "read timed out" ) ;
    }

}//*Socket::recv



/** Close the socket. **/
    void
Socket::close( void )
{
    //debug("B") ;
    int r ;

    r = ::close( sock_ ) ;
    if( r == -1 ) throw Socket::Exception( strerror(errno) ) ;
    is_conn_ = false ;
}//*Socket::close



/** Shutdown the socket.
 * @param[in] how 0-SHUT_RD , 1-SHUT_WR, 2-SHUT_RDWR. **/
void
Socket::shutdown( const int how )
{
    //debug("B") ;
    int r ;

    r = ::shutdown( sock_,  how ) ;
    if( r == -1 ) throw Socket::Exception( strerror(errno) ) ;
}//*Socket::shutdown



/** Set socket to sync or async socket.
 * @param[in] blocking If true, socket is blocking. **/
void
Socket::sync( bool blocking )
{
    int rv ;

    //! Set non-blocking, only if the socket is blocking
    if( !blocking && is_sync_  )
    {
        if( 0 > ( rv=fcntl(sock_,F_GETFL,NULL) ) )
            throw Exception( strerror(errno) ) ;

        rv |= O_NONBLOCK ;
        if( 0 > fcntl(sock_,F_SETFL,rv) )
            throw Exception( strerror(errno) ) ;
        is_sync_ = false ;
        return ;
    }

    //! Set blocking, only if the socket in non-blocking
    if( blocking && !is_sync_ )
    {
        if( 0 > ( rv=fcntl(sock_,F_GETFL,NULL) ) )
            throw Exception( strerror(errno) ) ;

        rv &= ~O_NONBLOCK ;
        if( 0 > fcntl(sock_,F_SETFL,rv) )
            throw Exception( strerror(errno) ) ;
        is_sync_ = true ;
        return ;
    }
}//* Socket::sync



#if 0
Socket::options( )
{
}

Socket::options( )
{
}
#endif

#if 0
setsockopt(int level, int optname, const void *optval, socklen_t optlen);
getsockopt(int level, int optname, void *optval,       socklen_t *optlen);
setoptions( Socket::Option::ReuseAddress )
    Socket::Type::Stream
    Socket::Type::Datagram
    Socket::Type::Raw
    Socket::Type::Unknown

    Socket::getoptions()
Socket::setoptions()

    Socket::bind()
    Socket::listen()
Socket::accept()
#endif
