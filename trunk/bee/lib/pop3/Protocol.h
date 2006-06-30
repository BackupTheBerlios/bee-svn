#if !defined POP3_H
#define POP3_H

#include <socket/Socket.h>
#include <string>

namespace Pop3 {
    class Protocol : public Socket {
        public:
            Protocol();
            ~Protocol();
            void write( const std::string& message) ;
            void read( ) ;
            void open ( const char* host, const int& port );
            void apop(void) ;
            void dele( unsigned long int ) ;
            void list( long int ) ;
            void noop( void ) ;
            void pass( const std::string& ) ;
            void quit( void ) ;
            void retr( long int ) ;
            void rset( void ) ;
            void stat( void ) ;
            void top( long int, long int ) ;
            void uidl( long int ) ;
            void user( const std::string& ) ;
    };
}

#endif //POP3_H
