#if !defined POP3_H
#define POP3_H

#include <socket/Socket.h>
#include <string>
#include "../report/Report.h"


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
            int  stat( int* mails, int* size ) ; // return the number of mails a user has in Inbox
            void top( long int, long int ) ;
            void uidl( long int ) ;
            void user( const std::string& ) ;
            void user( const int idx );
            void pass( const int idx );
        private:
            Timer   timer_ ;
            Report::Pop3*  report_ ;
    };
}

#endif //POP3_H
