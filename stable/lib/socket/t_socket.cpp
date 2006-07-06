#include "Socket.h"

int main(int argc, char* argv[]) 
{
    Socket s ;
    if( 3 > argc )
    {
        printf("%s hostname port\n", argv[0] ) ;
        return -1 ;
    }
    try {
        #define BUF_LEN 2000
        char* buffer = new char [BUF_LEN];

        s.open( Socket::Family::Inet, Socket::Type::Stream, 0 ) ;
        s.sync( true ) ;
        s.connect( argv[1], atoi(argv[2]) ) ;
        s.read( buffer, BUF_LEN ) ;
        buffer[strlen(buffer)-2] = '\0' ; //! get rid of the terminating \r\n
        std::cout << "--[" <<buffer <<"]--" <<std::endl ;
    }catch( Socket::Exception& e )
    {
        std::cout << e.what() <<std::endl ;
        if( s.is_open() ) s.close() ;
    }
    return 0 ;
}
