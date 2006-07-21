#if !defined _SCHEDULER_H_
#define _SCHEDULER_H_
#if defined DEBUG
    #include "../../Debug.h"
#endif

#include <list>
#include <vector>
#include <signal.h>
#include <semaphore.h>
#include <iostream>
#include "distribute/RateGen.h"


using namespace std;
class Scheduler
{
    public:
        //Timer timer ; // pls redenumeste ca sa nu se confunde cu timer_

        Scheduler( ) ;
        ~Scheduler() ;

        int     run() ;          //!< sem_wait( semaphore )
        void    semaphore( sem_t sem )
                { sem_ = sem ; } ;


        sem_t*  semaphore()
                { return &sem_ ; } ;
        
        void    callback( void (*notf)(union sigval sig) )
                { notify_fun = notf ; } ;

        double  elapsed()
                {   return elapsed_ ; } ;

        // This should _insert_ timE, and keep cronTab in a sorted state
        void    addTime( double timE ) ;


    private:
        void    print_time( ) ;
        void    show( ) ;
        void    (*notify_fun)( union sigval sigval ) ;

    private:
        sem_t        sem_ ;    //!< Notify the workers to start working.
        double       elapsed_ ;
        list<double> cronTab_ ;  //!< List of timeouts.
        RateGen*     g_ ;
};
#endif
