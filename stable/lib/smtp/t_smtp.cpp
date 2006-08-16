#include <iostream>
#include "smtp/Protocol.h"
#include <unistd.h>


int t_smtp()
{
    Smtp::Protocol s ;
    int eml ;
    eml = open( "test.eml", O_RDONLY ) ;
    if( -1 > eml )
    {
        printf( "error on opening file\n" ) ;
        return -1 ;
    }
    try
    {
         s.open( "localhost", 25 ) ;std::cout << s.response() ;
         s.greet( "ehlo cucu" ) ;   std::cout << s.response() ;
         s.mailFrom("user1") ;      std::cout << s.response() ;
         s.rcptTo("user1") ;        std::cout << s.response() ;
         s.beginData() ;
//         s.write( "Subject: test\r\n\r\n" );
         s.sendFile( eml ) ;
         s.write("\r\n", 2 ) ;
         s.endData() ;              std::cout << s.response() ;
    }catch( Socket::Exception& e )
    {
        printf( "ERROR: %s\n", e.what() ) ;
        if( s.is_open() ) s.close() ;
        close( eml ) ;
    }
    close( eml ) ;
    return 0;
}



int main()
{
    for( int i=0; i < 100 ; ++i ) t_smtp() ;
    return 0 ;
}
