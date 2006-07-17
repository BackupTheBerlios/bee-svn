#if !defined _REPORT_H_
#define _REPORT_H_
#include "timer/Timer.h"
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
        private:
            Timer* timer_ ;
            int    session_ ;
            FILE* f ;
    } ;
    class Pop3 {
    } ;
} ;
#endif // _REPORT_H_
