#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>
#include <pthread.h>

sem_t           semaphore ;
Scheduler::Cron cron( semaphore ) ;
void tick(union sigval sigval) ;

int main( int argc, char* argv[])
{
    if( argc <=3 )
    {
        printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }

    for( int i=atoi(argv[1]); i>0 ; --i )
        cron.addTime( i*atoi(argv[2]) ) ;   //! addTime parameter is in elapsed.

    cron.callback(tick) ;

    cron.refresh( 1 ) ;                     //! refresh time
    cron.start() ;                          //! we can start the cron now

    while( cron.elapsed() <= atoi( argv[3]) )
    {
        ;
    }
    return 0;
}

// This is runned regularly, at 1 sec.
// so it will increment cron.elapsed() too
void tick(union sigval sigval)
{
    cron.runJob();
    printf("Tick\n") ;
}

