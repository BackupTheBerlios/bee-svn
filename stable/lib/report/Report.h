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
            FILE* f ;
    } ;
} ;
#endif // _REPORT_H_
