#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <limits.h>  // LONG_MIN LONG_MAX RAND_MAX
#include <sys/time.h>

#include <errno.h>
#include <pthread.h>


/** Cron : Default constructor. **/
Scheduler::Scheduler()
{
    g_ = new RateGen( 12, 15 ) ;
    elapsed_ = 0 ;
    sem_init( &sem_ , 0 , 0 ) ;
}


/** Cron : Default destructor. **/
Scheduler::~Scheduler()
{
    delete g_ ;
}



void
Scheduler::addTime( double timE )
{
    //cronTab_.push_back( timE+elapsed_ ) ;
}



/** runJob : Decrement a semaphore, so a client thread is awaken. **/
    int
Scheduler::run( )
{
    double tm ;

    printf("+++ENTER JOB++++\n");
    print_time( ) ;
    timeval tv ;
    gettimeofday(&tv, 0 ) ;
    elapsed_ = tv.tv_sec + tv.tv_usec*0.000001 ;
    if( cronTab_.empty() ) { printf("running\n"); cronTab_.push_back( elapsed_+ g_->exponential() ) ; }

    show() ;
    printf("front=%f elapsed_=%f diff=%f\n", cronTab_.front(),elapsed_, elapsed_- cronTab_.front() ) ;
    if( elapsed_ >= cronTab_.front() )
    {
        tm = g_->exponential() ;
        printf("runJob exponential=%f, exponential=--%f--  elapsed_+exponential=%f\n", elapsed_, tm, elapsed_+tm ) ;
        sem_post( &sem_ ) ;
        cronTab_.pop_front();
        cronTab_.push_back( elapsed_ + tm ) ; // push_back another value

    }

    printf("---EXIT JOB--2--\n\n");
    return 0;
}//* Cron::runJob


/** print_time : Debug purpose function. **/
    void
Scheduler::print_time( )
{
    time_t      rawtime;
    struct tm*  timeinfo;

    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf( "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
    //fprintf( stderr, "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
}//* Cron::print_time


void
Scheduler::show( )
{
    list< double>::iterator it ;

    it = cronTab_.begin() ;
    for( ; it!=cronTab_.end() && *it != RAND_MAX; ++it )
        cout << *it <<" " ;
    cout <<endl ;
}//* Cron::show

