#if !defined _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>
#include <time.h>

class Timer {
    public:
        Timer() :elapsed_(0.0) {};
        ~Timer() {};
        void start( ) { gettimeofday( &tv_start_, (struct timezone *)0 ) ; elapsed_ = 0.0 ; } ;
        void stop( )  { gettimeofday( &tv_stop_, (struct timezone *)0 ) ; } ;
        float elapsed( )
        {
            return elapsed_ = tv_stop_.tv_sec - tv_start_.tv_sec +
                             (tv_stop_.tv_usec - tv_start_.tv_usec)*0.000001;
        } ;
    private:
        struct timeval tv_start_, tv_stop_ ;
        float  elapsed_ ;
} ;

#endif //_TIMER_H_
