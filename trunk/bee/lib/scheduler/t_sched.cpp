#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>

unsigned int elapsed =0 ;
sem_t semaphore ;
Scheduler::Cron cron( semaphore ) ;

void Scheduler::Cron::notify_fun (union sigval sigval)
{
    ++elapsed ;         //! could this be a member of cron ?
    cron.runJob();
}


int main( int argc, char* argv[])
{
    if( argc <=3 )
    {
        printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }

    //! add the clients.
    for( int i=1; i<atoi(argv[1]) ; ++i )
        cron.addTime( i*atoi(argv[2]) ) ;  //! addTime parameter is in elapsed.

    cron.refresh( 1*atoi(argv[2]) ) ;      //! refresh time(1 minute).
    cron.start() ;              //! start the cron.

    while( elapsed <= atoi( argv[3])  )
    {
        ;
    }
    return 0;
}
