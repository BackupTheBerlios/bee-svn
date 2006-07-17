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
}



/** Cron : Default constructor. **/
Scheduler::Scheduler(sem_t sem)
{
    semap_ = sem ;
}



/** Cron : Default destructor. **/
Scheduler::~Scheduler()
{
}



void
Scheduler::addTime( double timE )
{
    // Insert the element while keeping the list sorted
    list< double >::iterator it ;
    for( it = cronTab_.begin()
            ; it!= cronTab_.end()
            ; ++it )
        if( timE < *it ) {
            cronTab_.insert( it, timE+elapsed_ ) ;
            return;
        }
}



void
Scheduler::delTime(unsigned long timE )
{
    list<unsigned long>::iterator it ;
    pthread_mutex_lock(&mtx_) ;         //-- Lock before we add

    printf("delTime %i\n", timE );
    for( it = cronTab_.begin()
        ; it != cronTab_.end() && *it <= timE 
        ; )
    {
        if( (*it) == timE ) {
            it = cronTab_.erase( it ) ;
            continue;
        }
        it++ ;
    }
    pthread_mutex_unlock(&mtx_) ;       //-- Unlock
}//* Cron::delTime



/** runJob : Decrement a semaphore, so a client thread is awaken. **/
    int
Scheduler::run()
{
    list< unsigned long >::iterator it ;

    printf("+++ENTER JOB++++\n");
    print_time( ) ;

    for( it = cronTab_.begin()
       ; it != cronTab_.end() ; )
    {

        gettimeofday(&tv, 0 ) ;
        elapsed_ = tv.tv_sec + tv.tv_usec*0.000001 ;

        if( elapsed_ <= *it )
        {
            sem_post( &semap_ ) ;
            printf( "runJob %u\n", *it ) ;
            it  = cronTab_.erase(it);
            cronTab_.push_back( d.exponential() ) ; // push_back another value
            continue;
        }else
            return ;
        ++it ;
    }
    show() ;

    printf("---EXIT JOB----\n\n");
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
    list<unsigned long>::iterator it ;

    it = cronTab_.begin() ;
    for( ; it!=cronTab_.end() && *it != RAND_MAX; ++it )
        cout << *it <<" " ;
    cout <<endl ;
}//* Cron::show



/** refresh : sets the time at which crontab is inspected. **/
    int
Scheduler::refresh( unsigned long sec, unsigned long nano_sec )
{
    timeOut_.refresh( sec, nano_sec, cronTab_ ) ;
    return 0 ;
}//* Cron::refresh



/** restart : restart cron. **/
    int
Scheduler::restart( )
{
    //debug(  "B");

    stop();
    start();
    return 0;
}//* restart


//-------------------POSIX TIMERS----------------------------------------
/** Posix Timers. **/

#if defined LINUX
    int
Scheduler::start( )
{
    //debug(  "B");

    int k ;
    struct sigevent   sigev ;
    struct itimerspec itimer = { { timeOut_.current(), 0 }, { timeOut_.current(), 0 } } ;

    memset( &sigev, 0, sizeof(struct sigevent) ) ;

    sigev.sigev_notify            = SIGEV_THREAD ;
    sigev.sigev_notify_function   = notify_fun ;
    sigev.sigev_notify_attributes = NULL ;
    sigev.sigev_value.sival_int   = k ;

    if( timer_create( CLOCK_REALTIME, &sigev, &timer_) < 0 )
    {
        fprintf( stderr, "[%d]: %s\n", __LINE__, strerror(errno) ) ;
        exit(errno) ;
    }

    if( timer_settime( timer_, 0, &itimer, NULL) < 0 )
    {
        fprintf( stderr, "[%d]: %s\n", __LINE__, strerror(errno) ) ;
        exit(errno) ;
    }
    //timer.start() ;
    return 0 ;
}

    int
Scheduler::stop( ) //TODO: timeR could be private
{
    //debug(  "B");
    //timer.stop() ;
    if( timer_delete( timer_) )
        printf( "error: %s\n", strerror(errno) );
}
#endif//-------------------POSIX TIMERS----------------------------------------



//-------------------FreeBSD TIMERS----------------------------------------
/** FreeBSD Timers. **/
#if defined FREEBSD
// itimers
    int
install_vtalrm_handler()
{
    struct sigaction action;

    action.sa_handler = notify_fun;

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    return sigaction(SIGVTALRM, &action, NULL);
}

    int
Scheduler::start()
{
    struct itimerval it;

    install_vtalrm_handler( );

    it.it_value.tv_sec = timeOut_.current() ;
    it.it_value.tv_usec = 0 ;
    it.it_interval = it.it_value;
    setitimer(ITIMER_VIRTUAL, &it, NULL); // in example, retVal was "return setitimer( ) ;"
    return 0 ;
}

    int
Scheduler::stop()
{
    struct itimerval it;

    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
    it.it_interval = it.it_value;
    return (setitimer(ITIMER_VIRTUAL, &it, NULL));
}
#endif//-------------------FreeBSD TIMERS--------------------------------------




//-----------------------DOC/TODO----------------------------------------------
/**
* Creaza o tabela de timeouts, pentru fiecare user.
* Tabela poate fi creata, asa incat distributia timpilor
* de de pornire a clientilor sa fie exponentiala.
* Asta ar necesita o reactualizare a tabelei de "timeouts",
* odata la 1 secunda, presupunand ca granularitatea este 1sec.
*
* Stiu ca granularitatea timpului e de 1 secunda.
* deci distributia[ aplic f(k,lambda) ] tb calculata in acest timp,
* sa stiu cati clienti pornesc. */
/**
 * Scheduler::Start
 * goes through the job queue, and looks
 * at the .timeout memeber.
 * if current_time= .timeout then run
 * the current job callClient() ;
 * inialize the? timer */

