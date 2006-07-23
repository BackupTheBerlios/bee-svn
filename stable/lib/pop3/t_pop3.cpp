#include "pop3/Protocol.h"
#include <pthread.h>


using namespace std ;


void*
thrd_fun( void*) {
    char user[512]="user1";
    int i=1;

    for( int i=1; i<99; ++i )
    {
        Pop3::Protocol pop3 ;
        try
        {
            pop3.open( "localhost", 110 ) ;
            pop3.user( "user1" ) ;
            pop3.pass( "user1" ) ;
        }catch( Pop3::Protocol::Exception& e )
        {
            printf( "ERROR:%s\n", e.what() ) ;
            if( pop3.is_open() ) pop3.close() ;
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









