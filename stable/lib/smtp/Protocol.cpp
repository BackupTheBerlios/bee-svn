#include "smtp/Protocol.h"
#if !defined WIN32
    #include <sys/sendfile.h>
#endif
#include <unistd.h>

/*
 * Error counting
 * Each function tries to catch a socketException.
 * If any is caugth, then is rethrown, so the LoadGen will catch it,
 * and stop the UserEmulation in place */
/*
 * smtp.greet( );
 * Clasa Smtp i se seteaza un obiect de tip results( se poate gasi alt nume :)  care
 * contine numarul de calluri catre functii, si timpii lor.
 * �sta e modul in care face httpperf, adica ţine Rezultatele in memorie,
 * ca la sfârşit sa facă un dump al rezultatelor

#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
*/



/**
 * Default contructor. **/
Smtp::Protocol::Protocol( )
{
    //debug( "C" ) ;
    report_ = new Report::Smtp("localhost" ) ;
    report_->timer( &timer_ ) ;     //set the timer which is read by the reporter
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
    bool
Smtp::Protocol::read( )
{
    #define BUF_LEN 2048
    int code ;
    char buffer[ BUF_LEN ] ;
    timer_.start() ;
    Socket::read( buffer, BUF_LEN ) ;
    timer_.stop() ;
    sscanf( Socket::resp_.c_str(), "%i", &code) ;
    if ( code>= 400 )
    {
        fprintf( stderr, "SMTP ERROR:>%s", Socket::resp_.c_str() ) ;
        debug("ERROR:%s", Socket::resp_.c_str() ) ;
        throw Socket::Exception("Smtp error");
    }
}



/**
 * Open a connection. */
    void
Smtp::Protocol::open( const char* h, const unsigned int p  )
{
    timer_.start() ;
    try {
        Socket::open( Socket::Family::Inet, Socket::Type::Stream, 0 ) ;
        Socket::connect( h, p ) ;
        read() ;
    }catch(Socket::Exception& ex)
    {
        report_->openErr() ;
        throw ex;
    }
    timer_.stop() ;
    report_->open( ) ;
}



/**
 * Open a connection. **/
    void
Smtp::Protocol::open( sockaddr_in* dest )
{
    timer_.start() ;
    try {
        Socket::open( Socket::Family::Inet, Socket::Type::Stream, 0 ) ;
        Socket::connect( dest ) ;
        read();
    }catch(Socket::Exception& ex)
    {
        report_->openErr() ;
        throw ex;
    }
    timer_.stop() ;
    report_->open( ) ;
}


/* SMTP Protocol specific functions */

/**
 * Decide if we greet with ehlo or helo. **/
 // Doesn't analyze the response code.
    void
Smtp::Protocol::greet( const string& greeT )
{
    debug("greet: %s", greeT.c_str() ) ;
    timer_.start() ;
    try {
        write( greeT + "\r\n" ) ;
        read() ;
    }catch(Socket::Exception& ex)
    {
        report_->greetErr();
        throw ex ;
    }
    timer_.stop() ;
    report_->greet( ) ;
}



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
    debug("%s", buf ) ;
    timer_.start() ;
    try{
        write( buf) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->mailFromErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->mailFrom( ) ;
}



/**
 * Specify the sender. **/
    void
Smtp::Protocol::mailFrom( const char* userName )
{   //! @fixme bof for below
    char fmt[4096];

    sprintf( fmt, "MAIL FROM: %s\r\n", userName ) ;
    debug("%s", fmt ) ;
    timer_.start() ;
    try {
        write( fmt ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->mailFromErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->mailFrom( ) ;
}



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
    debug("%s", buf) ;
    timer_.start() ;
    try{
        write( buf ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->rcptToErr()  ;
        throw ex;
    }
    timer_.stop() ;
    report_->rcptTo( ) ;
}



/**
 * Specify the recipient. **/
    void
Smtp::Protocol::rcptTo( const char* userName )
{
    //FIXME: bof for below
    char fmt[ 1024 ] ;

    sprintf( fmt, "RCPT TO: %s\r\n", userName ) ;
    debug( "%s", fmt ) ;
    timer_.start() ;
    try{
        write( fmt ) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->rcptToErr( ) ;
        throw ex ;
    }
    timer_.stop() ;
    report_->rcptTo( ) ;
}



/**
 * Specify the recipient. **/
void
Smtp::Protocol::rcptTo( int rcptsz, rcpt_t rcptList[] )
{
    char fmt[1024] = {0};
    for(int i=0; i<rcptsz; ++i)
    {
        sprintf(fmt,"user%i@%s", rcptList[i].idx,(rcptList[i].local==true?"localdomain":"remotedomain")) ;// HARDCODED
        rcptTo( fmt ) ;
    }
}



/**
 * Send Mail content. **/
    void
Smtp::Protocol::beginData( )
{
    debug("DATA");
    timer_.start() ;
    try {
        write( "DATA\r\n" ) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->beginDataErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->beginData( ) ;
}



void
Smtp::Protocol::randomData( int msg_sz )
{
    int rb ;
    int sz = ( msg_sz < 8192 ) ? msg_sz : 8192 ;

    //generate the message here
    char* msg = (char*)malloc( sz*sizeof(char) ) ;
    memset( msg, 'a', sz*sizeof(char) ) ;
    beginData();
    do
    {
        sz = ( msg_sz < 8192 ) ? msg_sz : 8192 ;
        debug("DATA:+%i bytes", sz ) ;
        rb = ::write( sock_, msg, sz ) ;
        if( rb <= -1 ) throw Exception( strerror(errno) ) ;
    }while( msg_sz-=sz ) ;
    endData() ;
    free( msg ) ;
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



    void
Smtp::Protocol::sendFile( const char name[])
{
    int    rval     = 0,in_fd ;
    off_t  begin    = 0 ;
    struct stat stat_buf;

    in_fd = ::open(name, O_RDONLY ) ;
    if( !in_fd ) throw Exception( strerror(errno) ) ;  
    rval = fstat ( in_fd, &stat_buf);
    if( -1 == rval ) throw Exception( strerror(errno) ) ;
    rval = sendfile( sock_, in_fd, &begin, stat_buf.st_size);
    if( -1 == rval ) {::close(in_fd) ; throw Exception( strerror(errno) ) ; }
    ::close(in_fd) ;

}



/** **/
    void
Smtp::Protocol::endData()
{
    debug(".");
    timer_.start() ;
    try {
        write("\r\n.\r\n" ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->endDataErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->endData( ) ;
}



/**
 * Send quit and close connection. **/
    void
Smtp::Protocol::quit( )
{
    debug("quit");
    timer_.start() ;
    try {
        write( "QUIT\r\n" ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->quitErr() ;
        throw ex ;
    }
    close( ) ;
    timer_.stop() ;
    report_->quit( ) ;
}


// Implement the functions below. Dont forget about timer.start/stop, report

/**
 * Reset state. **/
    void
Smtp::Protocol::rset( void )
{
    timer_.start() ;
    write( "RSET\r\n") ;
    if(!read()) report_->rsetErr() ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::rset




/**
 * Verify username. **/
    void
Smtp::Protocol::vrfy( const std::string& useR )
{
    timer_.start() ;
    try {
        write( "VRFY " + useR) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        //report_->vrfyErr() ;//TODO
        throw ex ;
    }
    timer_.stop() ;
    report_->rset() ;
}




/**
 * . **/
    void
Smtp::Protocol::expn( const std::string& aliaS )
{
    timer_.start() ;
    write( "EXPN " + aliaS ) ;
    //if(!read()) report_->expnErr() ;
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
    //if(!read()) report_->noopErr() ;
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
    //if(!read()) report_->turnErr() ;
    timer_.stop() ;
    report_->rset() ;
}//* Smtp::Protocol::turn
