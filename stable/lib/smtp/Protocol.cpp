#include "smtp/Protocol.h"


/*
 * smtp.greet( );
 * Clasa Smtp i se seteaza un obiect de tip results( se poate gasi alt nume :)  care
 * contine numarul de calluri catre functii, si timpii lor.
 * ăsta e modul in care face httpperf, adica ţine Rezultatele in memorie,
 * ca la sfârşit sa facă un dump al rezultatelor

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
    report_ = new Report::Smtp("localhost" ) ;
}//*



/**
 * Default destructor. **/
Smtp::Protocol::~Protocol()
{
    //debug( "D" ) ;
    delete report_ ;
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
    report_->timer( &timer_ ) ;
    return true ;
}//* Smtp::Protocol::read



/**
 * Open a connection. **/
    void
Smtp::Protocol::open( const char* h, const unsigned int p )
{
    timer_.start() ;
    Socket::open( Socket::Family::Inet, Socket::Type::Stream, 0 ) ;
    Socket::connect( h, p ) ;                                         //! @todo: see if SMTP can connect
    read( ) ;// banner
    timer_.stop() ;
    report_->open( ) ;
}//* Smtp::Protocol::open



/**
 * Decide if we greet with ehlo or helo. **/
 // Doesn't analyze the response code.
    void
Smtp::Protocol::greet( const string& greeT )
{
    //debug( "greet=[%s]", greeT.c_str() ) ;

    timer_.start() ;
    write( greeT + "\r\n" ) ;//unsafe
    read( ) ;
    timer_.stop() ;
    report_->greet( ) ;
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

    timer_.start() ;
    write( buf) ;
    read( ) ;
    timer_.stop() ;
    report_->mailFrom( ) ;
}//* Smtp::Protocol::mailFrom



/**
 * Specify the sender. **/
    void
Smtp::Protocol::mailFrom( const char* userName )
{   //! @fixme bof for below
    char fmt[4096];

    sprintf( fmt, "MAIL FROM: %s\r\n", userName ) ;

    timer_.start() ;
    write( fmt ) ;
    read( ) ;
    timer_.stop() ;
    report_->mailFrom( ) ;
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

    timer_.start() ;
    write( buf ) ;
    read( ) ;
    timer_.stop() ;
    report_->rcptTo( ) ;
}//* Smtp::Protocol::rcptTo



/**
 * Specify the recipient. **/
    void
Smtp::Protocol::rcptTo( const char* userName )
{
    //FIXME: bof for below
    char fmt[ 1024 ] ;

    sprintf( fmt, "RCPT TO: %s\r\n", userName ) ;

    timer_.start() ;
    write( fmt ) ;
    read( ) ;
    timer_.stop() ;
    report_->rcptTo( ) ;
}//* Smtp::Protocol::rcptTo----------------------------------


void
Smtp::Protocol::rcptTo( int rcptsz, int rcptList[] )
{
    char fmt[1024] = {0};
    for(int i=0; i<rcptsz; ++i)
    {
        sprintf(fmt,"user%i", rcptList[i]) ;
        rcptTo( fmt ) ;
    }
}//* Smtp::Protocol::rcptTo-----------------------------------



/**
 * Send Mail content. **/
    void
Smtp::Protocol::beginData( )
{
    timer_.start() ;
    write( "DATA\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->beginData( ) ;
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
    //TODO:rval = sendfile( sock_, in_fd, &begin, stat_buf.st_size);
    if( -1 == rval ) throw Exception( strerror(errno) ) ;
}


/** **/
    void
Smtp::Protocol::endData()
{
    timer_.start() ;
    write("\r\n.\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->endData( ) ;
}//*





/**
 * Send quit and close connection. **/
    void
Smtp::Protocol::quit( )
{
    timer_.start() ;
    write( "QUIT\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->quit( ) ;
    close( ) ;
}//* Smtp::Protocol::quit


// Implement the functions below. Dont forget about timer.start/stop, report

/**
 * Reset state. **/
    void
Smtp::Protocol::rset( void )
{
    timer_.start() ;
    write( "RSET\r\n") ;
    read( ) ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::rset




/**
 * Verify username. **/
    void
Smtp::Protocol::vrfy( const std::string& useR )
{
    timer_.start() ;
    write( "VRFY " + useR) ;
    read( ) ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::vrfy




/**
 * . **/
    void
Smtp::Protocol::expn( const std::string& aliaS )
{
    timer_.start() ;
    write( "EXPN " + aliaS ) ;
    read( ) ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::expn




/**
 * Keeps connection alive. **/
    void
Smtp::Protocol::noop( void )
{
    timer_.start() ;
    write( "NOOP\r\n") ;
    read( ) ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::noop




/**
 * Still have to see abt this function. **/
    void
Smtp::Protocol::turn( void )
{
    timer_.start() ;
    write( "TURN\r\n") ;
    read( ) ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::turn