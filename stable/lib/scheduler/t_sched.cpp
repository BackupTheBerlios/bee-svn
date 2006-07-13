#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>
#include <pthread.h>

unsigned int elapsed =0 ;
sem_t semaphore ;
Scheduler::Cron cron( semaphore ) ;
void tick(union sigval sigval) ;
void* thread_fun(void*) ;

int main( int argc, char* argv[])
{
    if( argc <=3 )
    {
        printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }
    pthread_t p ;
    int running = 1 ;

    for( int i=atoi(argv[1]); i>0 ; --i )
        cron.addTime( i*atoi(argv[2]) ) ;   //! addTime parameter is in elapsed.
    cron.show() ;
    cron.callback(tick) ;

    cron.refresh( 1 ) ;                     //! refresh time
    cron.start() ;                          //! we can start the cron now

    while( elapsed <= atoi( argv[3])  )
    {
        ;
    }
    running = 0 ;
    return 0;
}


void tick(union sigval sigval)
{
    //cron.timer.tick( ) ;
    printf("Tick\n") ;
    cron.delTime(elapsed);
    cron.runJob();
    cron.show() ;
    ++elapsed ;
}

