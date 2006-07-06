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
}//_Pop3::Pop3::Protocol




/** Default destructor. ___*/
Pop3::Protocol::~Protocol()
{
}//_Pop3::Protocol



    void
Pop3::Protocol::open( const char* host, const int& port)
{
    Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
    connect( host, port);
    read( );
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
    write( "APOP \r\n" ) ;
    read( ) ;
}

    void
Pop3::Protocol::dele( unsigned long int msgNo )
{
    write( "DELE "+itoa(msgNo)+"\r\n" ) ;
    read( ) ;
}

    void
Pop3::Protocol::list( long int msgNo )
{
    if( msgNo < 0 )
    {   write( "LIST\r\n" ) ;
        read( ) ; // receive one line only
    }else
    {   write( "LIST "+itoa(msgNo)+"\r\n" ) ;
        read( ) ; // receive a multi line message ( Ends with . )
    }
}

    void
Pop3::Protocol::noop( void )
{
    write( "NOOP\r\n" ) ;
    read( ) ;
}

    void
Pop3::Protocol::pass( const std::string& passwd )
{
    write( "PASS "+passwd+"\r\n" ) ;
    read( ) ;
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
    write( "RETR "+itoa(msgNo)+"\r\n" ) ;
    read( ) ; // multiline response
}

    void
Pop3::Protocol::rset( void )
{
    write( "RSET\r\n" ) ;
    read( ) ;
}

    void
Pop3::Protocol::stat( void )
{
    write( "STAT\r\n" ) ;
    read( ) ; //+OK number_of_mess maildrop_size(octets)
}

    void
Pop3::Protocol::top( long int msgNo, long int n )
{
    write( "TOP "+itoa(msgNo)+" "+itoa(n)+"\r\n" ) ;
    read( ) ;// if no err is returned, expect a multinile response
}

    void
Pop3::Protocol::uidl( long int msgNo )
{
    if( msgNo <0 )
    {   write( "UIDL\r\n" ) ;
        read( ) ; //multiline
    }else // it can be = 0 ??
    {   write( "UIDL "+itoa(msgNo)+"\r\n" ) ;
        read( ) ; //single line. extract the UniqueID
    }
}

    void
Pop3::Protocol::user( const std::string& userName )
{
    write( "USER "+userName+"\r\n" ) ;
    read( ) ;
}
