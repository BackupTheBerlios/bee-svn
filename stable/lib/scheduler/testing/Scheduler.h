#if !defined _SCHEDULER_H_
#define _SCHEDULER_H_

#include <vector>
#include <signal.h>
#include <semaphore.h>

#include <sys/signal.h>

using std::vector ;



namespace Scheduler
{

    class Cron
    {
        public:
            //Timer timer ; // pls redenumeste ca sa nu se confunde cu timer_

            Cron(sem_t sem ) ;
            ~Cron() ;

            int start() ;           //!< create and initialize timer_.
            int stop() ;            //!< delete timer_.
            int restart() ;         //!< restart the timer.( stop+start )
            int runJob() ;          //!< sem_wait( semaphore )
            int refresh( unsigned long timE ) ;
            void addTime( unsigned long timE ) { cronTab_.push_back(elapsed_ + timE) ; } ;
            void callback( void (*notf)(union sigval sig) )
            {   notify_fun = notf ; }

        private:
            sem_t   semap_ ;    //!< semaphore
            TimeOut timeOut_ ;  //!< The minimum time till the next job.
            vector<double>   cronTab_ ;  //!< List of timeouts.
            timer_t timer_ ;

        private:
            void print_time( ) ;
            void (*notify_fun)( union sigval sigval ) ;

    };
} // namespace



#endif
