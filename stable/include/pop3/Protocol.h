#if !defined POP3_H
#define POP3_H
#if defined DEBUG
    #include "Debug.h"
#endif

#include "Socket.h"
#include <string>
#include "Report.h"


namespace Pop3 {
    class Protocol : public Socket {
        public:
            Protocol();
            ~Protocol();
            void write( const std::string& message) ;
            void read( ) ;
            void open ( const char* host, const int& port );
            void open ( sockaddr_in* dest ) ;
            void apop(void) ;
            void dele( unsigned long int ) ;
            void list( long int ) ;
            void noop( void ) ;
            void pass( const std::string& ) ;
            void quit( void ) ;
            void retr( long int ) ;
            void rset( void ) ;
            void  stat( int* mails, int* size ) ;
            void top( long int, long int ) ;
            void uidl( long int ) ;
            void user( const std::string& ) ;
            void user( const char* prefix, const int idx );
            void pass( const char* prefix, const int idx );
        private:
            Timer   timer_ ;
            Report::Pop3*  report_ ;
    };
}

#endif //POP3_H
