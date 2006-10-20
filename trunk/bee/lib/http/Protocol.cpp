#include "Protocol.h"
#include <string>
#include <iostream>
#include <stdlib.h> // itoa
#include <string.h>

using namespace std;



/** integer to string. **/
string itoa( int a )
{
    char str[33] ;
    sprintf( str, "%i", a ) ;
    return string(str) ;
}//* itoa




/** Default constructor. **/
Http::Protocol::Protocol()
{
    ver_.major = 1 ;
    ver_.minor = 1 ;
}//* Http::Http::Protocol



/** Default destructor. **/
Http::Protocol::~Protocol()
{
}//* Http::Protocol

//! set HTTP version
    inline void
version( int major, int minor )
{
    ver_.major = major;
    ver_.minor = minor ;
}

//! get the current version
    inline void
Http::Protocol::version( int* major, int* minor )
{
    *major = ver_.major ;
    *minor = ver_.minor ;
}


/** Open a connection, as a step by step solution. **/
    void
Http::Protocol::open( const char* host, const int& port)
{
    Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
    connect( host, port);
    //      read( );
}//*Http::Protocol::open




/** Read from socket. **/
    void
Http::Protocol::read()
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
        {   fprintf( stderr, "HttpProtocol ERROR\n") ;
            return ;
        }
    }
}//* Protocol::read



/** Send message to server. **/
    void
Http::Protocol::write( const std::string& message)
{
    cout <<message <<endl;//debug
    Socket::write( message ) ;
}//* Protocol::write



/** Request data from server. **/
    void
Http::Protocol::get( const char* url )
{
    //try_connect
    struct Url p ;
    char req[1024] ;
    parse_url( url, &p ) ;
    open( p.host , p.port ) ;
    sprintf( req, "GET %s HTTP/%i.%i", p.abs_path, ver_.major, ver_.minor );
    write( req ) ;
    //    write( "Host: " + string(p.host) ) ;
    write( "\r\n\r\n" ) ;
}//* Protocol::get



/** Parse a URL, and extract an Url structure. **/
    bool
Http::Protocol::parse_url( const char* url, struct Url* purl )
{
#define HTTP_LEN 7
    char    port[5] ={0} ;
    char    hp[1024]={0} ;
    char*   m1 ;        //! returned pointer
    char*   m2 ;        //! returned pointer

    // Test if URL starts with http://
    if( url - strstr( url, "http://" ) )  { return false ; }

    //                         m1
    //                         |
    // http://hostname.com:9000/absolute_path.html
    m1 = strchr( url+HTTP_LEN, '/' ) ;
    if( !m1 )     { return false ; }
    strcpy( purl->abs_path, m1 ) ;

    // hp         m2
    // |           |
    // hostname.com:9000
    strncpy( hp, url+HTTP_LEN, m1 - (url+HTTP_LEN ) ) ;
    if( (m2=strrchr(hp, ':')) ) // Port is specified
    {
        strcpy (      port, m2+1 ) ;
        strncpy(purl->host,  hp, m2 - hp ) ;
        purl->port = atoi(port) ;
    }else
        strncpy( purl->host, hp, m1 - (url+HTTP_LEN) ) ;
    return true ;
}//* Protocol::parse_url



/** Retrieve only the headers. **/
    void
Http::Protocol::head( const char* what )
{
    //extract host
    //extract port
    //extract file
    //try_connect
    write( "GET" ) ;
}//* Protocol::head



/** Send data to server. **/
    void
Http::Protocol::post( const char* what )
{
    //write( "POST " + what + "HTTP/" + ver_.major + "." + ver_.minor ) ;
}//* Protocol::post
