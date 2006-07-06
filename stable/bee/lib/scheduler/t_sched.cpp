#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>

unsigned int elapsed =0 ;

sem_t semaphore ;



Scheduler::Cron cron( semaphore ) ;

void tick(union sigval sigval) ;


int main( int argc, char* argv[])
{
    if( argc <=3 ) { printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }

    cron.callback(tick) ;

    for( int i=1; i<atoi(argv[1]) ; ++i )
        cron.addTime( i*atoi(argv[2]) ) ;   //! addTime parameter is in elapsed.

    cron.refresh( 1*atoi(argv[2]) ) ;       //! refresh time
    cron.start() ;                          //! we can start the cron now

    while( elapsed <= atoi( argv[3])  )
    {
        ;
    }
    return 0;
}


void tick(union sigval sigval)
{
    //cron.timer.tick( ) ;
    cron.runJob();
}

