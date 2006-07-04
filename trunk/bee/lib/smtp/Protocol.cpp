#include "smtp/Protocol.h"

#if defined USE_DEBUG
    #include "common/Debug.h"
#endif

#if !defined WIN32
    #include <sys/sendfile.h>
#endif


/**
 * Default contructor. **/
Smtp::Protocol::Protocol( )
{
    //debug( "C" ) ;
    //_report = new Report::Client("localhost" ) ;
}//*



/**
 * Default destructor. **/
Smtp::Protocol::~Protocol()
{
    //debug( "D" ) ;
    //delete _report ;
}//*



/**
 * Get and tests, a smtp response. **/
// 250 >>7 = 1
// 354 >>7 = 2
// 451 >>7 = 3
// 500 >>7 = 3
    bool
Smtp::Protocol::read( )
{
    #define BUF_LEN 2048
    int code ;
    char buffer[ BUF_LEN ] ;

    Socket::read( buffer, BUF_LEN ) ;
    sscanf( Socket::resp_.c_str(), "%i", &code) ;
    if ( code>>7 == 3 )
    {
        printf( "SMTP ERROR:>%s", Socket::resp_.c_str() ) ;
        return false ;
    }
    return true ;
}//* Smtp::Protocol::read



/**
 * Open a connection. **/
    void
Smtp::Protocol::open( const char* h, const unsigned int p )
{
    Socket::open( Socket::Family::Inet, Socket::Type::Stream, 0 ) ;
    Socket::connect( h, p ) ;                                         //! @todo: see if SMTP can connect
    read( ) ;// banner
}//* Smtp::Protocol::open



/**
 * Decide if we greet with ehlo or helo. **/
 // Doesn't analyze the response code.
    void
Smtp::Protocol::greet( const string& greeT )
{
    //debug( "greet=[%s]", greeT.c_str() ) ;

    //_timer.start() ;
    write( greeT + "\r\n" ) ;//unsafe
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::greet



/**
 * Specify the sender name@domain. **/
// Doesn't analyze the response code
    void
Smtp::Protocol::mailFrom( const char* userFormaT, const unsigned int useR,
        const char* domainFormaT, const unsigned int domaiN )
{   //! @fixme bof for below
    char buf[ 1024 ] ;
    char fmt[ 1024 ] ;

    sprintf( fmt, "MAIL FROM: %s@%s\r\n", userFormaT, domainFormaT ) ;
    sprintf( buf, fmt, useR, domaiN ) ;

    //_timer.start() ;
    write( buf) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::mailFrom



/**
 * Specify the sender. **/
    void
Smtp::Protocol::mailFrom( const char* userName )
{   //! @fixme bof for below
    char fmt[4096];

    sprintf( fmt, "MAIL FROM: %s\r\n", userName ) ;

    //_timer.start() ;
    write( fmt ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::mailFrom



/**
 * Specify the recipient. **/
// Doesn't analyze the response code.
    void
Smtp::Protocol::rcptTo( const char* userFormaT, const unsigned int useR,
        const char* domainFormaT, const unsigned int domaiN )
{
    //FIXME: bof for below
    char buf[ 1024 ] ;
    char fmt[ 1024 ] ;

    sprintf( fmt, "RCPT TO: %s@%s\r\n", userFormaT, domainFormaT ) ;
    sprintf( buf, fmt, useR, domaiN ) ;

    //_timer.start() ;
    write( buf ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::rcptTo



/**
 * Specify the recipient. **/
    void
Smtp::Protocol::rcptTo( const char* userName )
{
    //FIXME: bof for below
    char fmt[ 1024 ] ;

    sprintf( fmt, "RCPT TO: %s\r\n", userName ) ;

    //_timer.start() ;
    write( fmt ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::rcptTo



/**
 * Send Mail content. **/
    void
Smtp::Protocol::beginData( )
{
    //_timer.start() ;
    write( "DATA\r\n" ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//* Smtp::Protocol::data

void
Smtp::Protocol::randomData( int msg_sz )
{
    int rb ;
    int sz = ( msg_sz < 8192 ) ? msg_sz : 8192 ;
    char buf[sz+1] ;


    beginData();
    //generate the message here
    int fd = ::open("/dev/urandom", O_RDONLY ) ;
    do
    {
        rb = ::read( fd, buf, sz ) ;
        buf[rb] = '\0' ;
        write(buf, rb) ;
        write("\n" ) ;
    }while( msg_sz-=rb ) ;
    endData() ;
    ::close(fd) ;
}

/**
 * @param in_fd : Must be an opened file descriptor. **/
    void
Smtp::Protocol::sendFile( int in_fd )
{
    int    rval     = 0 ;
    off_t  begin    = 0 ;
    struct stat stat_buf;

    rval = fstat ( in_fd, &stat_buf);
    if( -1 == rval ) throw Exception( strerror(errno) ) ;
    rval = sendfile( sock_, in_fd, &begin, stat_buf.st_size);
    if( -1 == rval ) throw Exception( strerror(errno) ) ;
}


/** **/
    void
Smtp::Protocol::endData()
{
    //_timer.start() ;
    write("\r\n.\r\n" ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
}//*





/**
 * Send quit and close connection. **/
    void
Smtp::Protocol::quit( )
{
    //_timer.start() ;
    write( "QUIT\r\n" ) ;
    read( ) ;
    //_timer.stop() ;
    //_report->write( pthread_self(), __FUNCTION__, //_timer.elapsed() ) ;
    close( ) ;
}//* Smtp::Protocol::quit


// Implement the functions below. Dont forget about timer.start/stop, report

/**
 * Reset state. **/
    void
Smtp::Protocol::rset( void )
{
    write( "RSET\r\n") ;
    read( ) ;
}//* Smtp::Protocol::rset




/**
 * Verify username. **/
    void
Smtp::Protocol::vrfy( const std::string& useR )
{
    write( "VRFY " + useR) ;
    read( ) ;
}//* Smtp::Protocol::vrfy




/**
 * . **/
    void
Smtp::Protocol::expn( const std::string& aliaS )
{
    write( "EXPN " + aliaS ) ;
    read( ) ;
}//* Smtp::Protocol::expn




/**
 * Keeps connection alive. **/
    void
Smtp::Protocol::noop( void )
{
    write( "NOOP\r\n") ;
    read( ) ;
}//* Smtp::Protocol::noop




/**
 * Still have to see abt this function. **/
    void
Smtp::Protocol::turn( void )
{
    write( "TURN\r\n") ;
    read( ) ;
}//* Smtp::Protocol::turn
