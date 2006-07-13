#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>
#include <pthread.h>
#include <sys/time.h>

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
    timeval tv ;
    gettimeofday(&tv, 0 ) ;
    srand( tv.tv_usec ) ;

    for( int i=atoi(argv[1]); i>0 ; --i )
    {
        int c = rand() ;
        int t = (int)(c*(1.0*atoi(argv[3]))/RAND_MAX) + 1 ;
        cron.addTime( i*t ) ;   //! addTime parameter is in elapsed.
    }

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
    printf("Tick\n") ;
    cron.runJob();
}

