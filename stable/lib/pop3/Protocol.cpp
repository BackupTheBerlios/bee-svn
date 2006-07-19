#include "pop3/Protocol.h"
#include <string>
#include <iostream>
#include <stdlib.h> // itoa

using namespace std;

string itoa( int a )
{
    char str[33] ;
    sprintf( str, "%i", a ) ;
    return string(str) ;
}

/** Default constructor. ___*/
Pop3::Protocol::Protocol()
{
    report_ = new Report::Pop3() ;
    report_->timer( &timer_ ) ;
}//_Pop3::Pop3::Protocol




/** Default destructor. ___*/
Pop3::Protocol::~Protocol()
{
    delete report_ ;
}//_Pop3::Protocol



    void
Pop3::Protocol::open( const char* host, const int& port)
{
    timer_.start() ;
    Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
    connect( host, port);
    read( );
    timer_.stop() ;
    report_->open() ;
}//_Pop3::Protocol::open

    void
Pop3::Protocol::open(sockaddr_in* dest ) 
{
    timer_.start() ;
    Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
    connect(dest) ;
    read( );
    timer_.stop() ;
    report_->open() ;
}//_Pop3::Protocol::open



    void
Pop3::Protocol::write( const std::string& message)
{
    cout <<message <<endl;//debug
    Socket::write( message ) ;
}

    void
Pop3::Protocol::read()
{
#define BUF_LEN 2048
    char buffer[BUF_LEN] ;

    for( ;;)
    {
        Socket::read( buffer, BUF_LEN ) ;
        if(!resp_.empty() )
            cout <<resp_ <<endl ; //debug

        if(resp_.find( "+OK ",0) != string::npos)
        {   printf("-OK\n" ) ;
            return ;
        }
        if(resp_.find("-ERR ",0) != string::npos)
        {   printf("ERROR\n") ;
            return ;
        }
    }
}


    void
Pop3::Protocol::apop( void )
{
    timer_.start() ;
    write( "APOP \r\n" ) ;
    read( ) ;
    timer_.stop() ;
    //report_->apop() ; // TODO
}

    void
Pop3::Protocol::dele( unsigned long int msgNo )
{
    timer_.start() ;
    write( "DELE "+itoa(msgNo)+"\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->dele() ;
}

    void
Pop3::Protocol::list( long int msgNo )
{
    if( msgNo < 0 )
    {
        timer_.start() ;
        write( "LIST\r\n" ) ;
        read( ) ; // receive one line only
        timer_.stop() ;
        //report_->list() ;//TODO
    }else
    {
        timer_.start() ;
        write( "LIST "+itoa(msgNo)+"\r\n" ) ;
        read( ) ; // receive a multi line message ( Ends with . )
        timer_.stop() ;
        //report_->list() ;//TODO
    }
}

    void
Pop3::Protocol::noop( void )
{
    timer_.start() ;
    write( "NOOP\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    //report_->noop() ;//TODO
}

    void
Pop3::Protocol::pass( const std::string& passwd )
{
    timer_.start() ;
    write( "PASS "+passwd+"\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->pass() ;
}

    void
Pop3::Protocol::quit( void )
{
    write( "QUIT\r\n" ) ;
    read( ) ;
}

    void
Pop3::Protocol::retr( long int msgNo )
{
    timer_.start() ;
    write( "RETR "+itoa(msgNo)+"\r\n" ) ;
    read( ) ; // multiline response
    timer_.stop() ;
    report_->retr() ;
}

    void
Pop3::Protocol::rset( void )
{
    timer_.start() ;
    write( "RSET\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    //report_->rset() ;//TODO
}

    int
Pop3::Protocol::stat( int* mails, int* size )
{
    timer_.start() ;
    write( "STAT\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->stat() ;
    sscanf( resp_.c_str(), "+OK %i %i", mails, size ) ;
    return 0 ;
}

    void
Pop3::Protocol::top( long int msgNo, long int n )
{
    timer_.start() ;
    write( "TOP "+itoa(msgNo)+" "+itoa(n)+"\r\n" ) ;
    read( ) ;// if no err is returned, expect a multinile response
    timer_.stop() ;
    //report_->top() ;//TODO
}

    void
Pop3::Protocol::uidl( long int msgNo )
{
    if( msgNo <0 )
    {
        timer_.start() ;
        write( "UIDL\r\n" ) ;
        read( ) ; //multiline
        timer_.stop() ;
        //report_->uidl() ;//TODO
    }else // it can be = 0 ??
    {
        timer_.start() ;
        write( "UIDL "+itoa(msgNo)+"\r\n" ) ;
        read( ) ; //single line. extract the UniqueID
        timer_.stop() ;
        //report_->uidl() ;//TODO
    }
}

    void
Pop3::Protocol::user( const std::string& userName )
{
    timer_.start() ;
    write( "USER "+userName+"\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    report_->user() ;
}


/*
 * LIMIT: command line length is 512 chars */
    void
Pop3::Protocol::user( const char* prefix, const int idx )
{
    char buf[512] ={0};
    sprintf( buf, "USER %s%i\r\n", prefix, idx );
    timer_.start() ;
    write( buf ) ;
    read( ) ;
    timer_.stop() ;
    report_->user() ;
}




/*
 * LIMIT: command line length is 512 chars */
    void
Pop3::Protocol::pass( const char* prefix, const int idx )
{
    char buf[512] ={0};
    sprintf( buf, "PASS %s%i\r\n", prefix, idx );
    timer_.start() ;
    write( buf ) ;
    read( ) ;
    timer_.stop() ;
    report_->user() ;
}
