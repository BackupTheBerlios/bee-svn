#if !defined _SCHEDULER_H_
#define _SCHEDULER_H_

#include <vector>
#include <signal.h>
#include <semaphore.h>

#include <sys/signal.h>

using std::vector ;

/* FreeBSD does not define sigval_t */
typedef union sigval sigval_t;

namespace Scheduler
{
    class TimeOut
    {
        public:
            TimeOut() ;
            ~TimeOut() ;
            void min( int miN ) { min_ = miN ; } ;
            void max( int maX ) { max_ = maX ; } ;
            void current( int currenT ) { cur_ = currenT ; } ;

            int  min()      { return min_ ; } ;
            int  max()      { return max_ ; } ;
            int  current()  { return cur_ ; } ;
            void refresh( int newTimE, vector< unsigned long > timeLisT ) ;

        private:
            int min_, max_, cur_ ;
            unsigned long gcm( unsigned long x, unsigned long y ) ;
    } ;


    class Cron
    {
        public:
            //Timer timer ; // pls redenumeste ca sa nu se confunde cu timer_

            Cron( ) {};
            Cron(sem_t sem ) ;
            ~Cron() ;
            void semaphore( sem_t sem ){ semap_ = sem ; } ;
            int start() ;           //!< create and initialize timer_.
            int stop() ;            //!< delete timer_.
            int restart() ;         //!< restart the timer.( stop+start )
            int runJob() ;          //!< sem_wait( semaphore )
            int refresh( unsigned long timE ) ;
            void addTime( unsigned long timE ) { cronTab_.push_back(timE) ; } ;
            void callback( void (*notf)(union sigval sig) )
            {   notify_fun = notf ; }

        private:
            sem_t   semap_ ;    //!< semaphore
            TimeOut timeOut_ ;  //!< The minimum time till the next job.
            vector<unsigned long>   cronTab_ ;  //!< List of timeouts.
            timer_t timer_ ;

        private:
            void print_time( ) ;
            void (*notify_fun)( union sigval sigval ) ;

    };
} // namespace



#endif
