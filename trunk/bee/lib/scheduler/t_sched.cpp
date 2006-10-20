#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <ctime>
#include <pthread.h>
#include <sys/time.h>

int main( int argc, char* argv[])
{
    if( argc <=3 )
    {
        printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }

    Scheduler::Scheduler cron ;


    timeval tv ;
    gettimeofday(&tv, 0 ) ;
    srand( tv.tv_usec ) ;
    while( cron.elapsed() <= tv.tv_sec+atoi( argv[3]) )
    {
        printf("cron.elapsed() =%f\n", cron.elapsed() ) ;
        cron.run() ;
    }
    return 0;
}
