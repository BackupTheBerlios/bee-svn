#if !defined _SCHEDULER_H_
#define _SCHEDULER_H_

#include <list>
#include <vector>
#include <signal.h>
#include <semaphore.h>
#include <iostream>

#include <sys/signal.h>

using namespace std;
class Scheduler
{
    public:
        //Timer timer ; // pls redenumeste ca sa nu se confunde cu timer_

        Scheduler( ) ;
        Scheduler(sem_t sem ) ;
        ~Scheduler() ;
        int     start() ;           //!< create and initialize timer_.
        int     stop() ;            //!< delete timer_.
        int     restart() ;         //!< restart the timer.( stop+start )
        int     runJob() ;          //!< sem_wait( semaphore )
        int     refresh( unsigned long sec, unsigned long nano_sec ) ;
        void    semaphore( sem_t sem ){ semap_ = sem ; } ;

        void    callback( void (*notf)(union sigval sig) )
        {   notify_fun = notf ; } ;

        unsigned long elapsed()
        {   return timeOut_.current() ; } ;

        // This should _insert_ timE, and keep cronTab in a sorted state
        void    addTime( unsigned long timE ) ;

        // Keep cronTab sorted, so delTime takes less time.
        void    delTime( unsigned long timE ) ;


    private:
        void    print_time( ) ;
        void    show( ) ;
        void    (*notify_fun)( union sigval sigval ) ;

    private:
        sem_t   semap_ ;    //!< Notify the workers to start working.
        double  elapsed_ ;
        list<double> cronTab_ ;  //!< List of timeouts.
};
#endif
