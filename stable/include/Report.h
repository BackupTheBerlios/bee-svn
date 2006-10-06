#if !defined _REPORT_H_
#define _REPORT_H_
#if defined DEBUG
    #include "Debug.h"
#endif

#include "Timer.h"
#include <cstdio>

namespace Report {
    class Smtp {
        public:
            Smtp() ;
            Smtp(const char* host) ;
            ~Smtp() ;
            void timer( Timer* timer ) { timer_ = timer ; } ;
            void session( const int s ) { session_ = s ; } ;
            void open() ;
            void greet() ;
            void mailFrom() ;
            void rcptTo() ;
            void beginData() ;
            void endData() ;
            void quit() ;
            void rset() ;
            void openErr() ;
            void greetErr() ;
            void mailFromErr() ;
            void rcptToErr() ;
            void beginDataErr() ;
            void endDataErr() ;
            void quitErr() ;
            void rsetErr() ;
        private:
            Timer* timer_ ;
            int    session_ ;
            int  out ;
    } ;
    class Pop3 {
        public:
            Pop3() ;
            ~Pop3() ;
            void timer( Timer* timer ) { timer_ = timer ; } ;
            void session( const int s ) { session_ = s ; } ;
            void open() ;
            void user() ;
            void pass() ;
            void stat() ;
            void retr() ;
            void dele() ;
            void quit() ;
            void openErr() ;
            void userErr() ;
            void passErr() ;
            void statErr() ;
            void retrErr() ;
            void topErr() ;
            void deleErr() ;
            void quitErr() ;
        private:
            Timer* timer_ ;
            int    session_ ;
            int  out ;
    } ;
} ;
#endif // _REPORT_H_
