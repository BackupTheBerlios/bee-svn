#include "Scheduler.h"
#if defined USE_DEBUG
#include "common/Debug.h"
#endif
#include <limits.h>  // LONG_MIN LONG_MAX
#include <sys/time.h>

#include <errno.h>



Scheduler::Scheduler(sem_t sem)
{
    semap_ = sem ;     // folosit sa notifice "the hurd"
}



Scheduler::~Scheduler()
{
}



/** Decrement a semaphore, so a client thread is awaken. **/
    int
Scheduler::run()
{
    vector< double >::iterator it ;
    gettimeofday( &tv, 0 ) ;
    elapsed_ = tv.tv_sec*1000000+tv.tv_usec ;
    print_time( ) ;
    for( it  = cronTab_.begin( ); it != cronTab_.end(); ++it )
    {
        if( elapsed_ == *it )
        {
            sem_post(&semap_) ;
            cronTab_.erase(it) ;
            cronTab_.push_back(elapsed_ + d.exponential() ) ; // define a RateGen object, named d
            printf("runJob %u\n", *it ) ;
        }
    }
    printf("-------\n\n");
    return 0;
}



/** print_time : Debug purpose function. **/
    void
Scheduler::print_time( )
{
    time_t      rawtime;
    struct tm*  timeinfo;

    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf( "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
    fprintf( stderr, "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
}//* Cron::print_time
