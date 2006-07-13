#if !defined _SCHEDULER_H_
#define _SCHEDULER_H_

#include <list>
#include <vector>
#include <signal.h>
#include <semaphore.h>
#include <iostream>

#include <sys/signal.h>

using namespace std;

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
            void refresh( int newTimE, list< unsigned long > timeLisT ) ;

        private:
            int min_, max_, cur_ ;
            unsigned long gcm( unsigned long x, unsigned long y ) ;
    } ;


    class Cron
    {
        public:
            //Timer timer ; // pls redenumeste ca sa nu se confunde cu timer_

            Cron( ) ;
            Cron(sem_t sem ) ;
            ~Cron() ;
            void    semaphore( sem_t sem ){ semap_ = sem ; } ;
            int     start() ;           //!< create and initialize timer_.
            int     stop() ;            //!< delete timer_.
            int     restart() ;         //!< restart the timer.( stop+start )
            int     runJob() ;          //!< sem_wait( semaphore )
            int     refresh( unsigned long timE ) ;


            void    callback( void (*notf)(union sigval sig) )
            {   notify_fun = notf ; } ;


            // This should _insert_ timE, and keep cronTab in a sorted state
            void    addTime( unsigned long timE ) ;

            // Keep cronTab sorted, so delTime takes less time.
            void    delTime( unsigned long timE ) ;


        private:
            pthread_mutex_t     mtx_ ;
            sem_t               semap_ ;    //!< semaphore
            TimeOut             timeOut_ ;  //!< The minimum time till the next job.
            timer_t             timer_ ;
            list<unsigned long> cronTab_ ;  //!< List of timeouts.

        private:
            void    print_time( ) ;
            void    show( ) ;
            void (*notify_fun)( union sigval sigval ) ;

    };
}; // namespace



#endif
