#if !defined PROTOCOL_H
#define PROTOCOL_H

#include "socket/Socket.h"
//#include "common/Timer.h"
//#include "common/Report.h"

namespace Smtp {
    class Protocol: public Socket
    {
        public:
            Protocol    ( ) ;
            ~Protocol   ( ) ;
            bool read   ( ) ;
            void sendFile( int in_fd ) ;
            void open   ( const char* host, const unsigned int port ) ;
            void greet  ( const std::string& greet) ;
            void vrfy   ( const std::string& useR) ;
            void expn   ( const std::string& aliaS) ;
            void rset   ( void ) ;
            void noop   ( void ) ;
            void turn   ( void ) ;
            void quit   ( void ) ;

            void mailFrom( const char* userName ) ;
            void mailFrom( const char* userFormat,   const unsigned int userIdx,
                    const char* domainFormat, const unsigned int domainIdx ) ;

            void rcptTo ( const char* userName ) ;
            void rcptTo ( const char* userFormat,   const unsigned int userIdx,
                    const char* domainFormat, const unsigned int domainIdx ) ;
            void beginData() ;
            void randomData( int msg_sz ) ;
            void endData() ;
        private:
            //Timer   _timer ;
            //Report::Client*  _report ;
    } ;
}
#endif  //PROTOCOL_H
