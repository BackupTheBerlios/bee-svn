#include "Protocol.h"
#include <pthread.h>


using namespace std ;


void*
thrd_fun( void*) {
    int i=1;

    for( int i=1; i<99; ++i )
    {
        Http::Protocol http ;
        try
        {
//            http.open( "www.google.com", 80 ) ;
//            http.write( "GET / HTTP/1.1\r\n\r\n" ) ;
            http.get( "http://www.google.com/index.html" ) ;
            http.read( ) ;
            cout <<http.response() <<endl ;

            //SAU
            //http.get( "http://localhost:9000/index.html" )
            //http.head( "http://localhost:9000/index.html" )
            //
            //fara sa mai fac open
        }catch( Http::Protocol::Exception& e )
        {
            printf( "ERROR:%s\n", e.what() ) ;
            if( http.is_open() ) http.close() ;
        }
        return 0 ;
    }
}


    int
main()
{
    thrd_fun( NULL ) ;
    return 0 ;
}

