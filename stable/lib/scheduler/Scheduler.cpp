#include "Scheduler.h"
#if defined USE_DEBUG
    #include "common/Debug.h"
#endif
#include <limits.h>  // LONG_MIN LONG_MAX
#include <sys/time.h>

#include <errno.h>
#include <pthread.h>

#if !defined LINUX
    typedef int sigval_t ;
#endif


/** Cron : Default constructor. **/
Scheduler::Cron::Cron()
{
    //debug(  "C" ) ;
    if( cronTab_.empty() ) cronTab_.push_back(RAND_MAX) ; // so that addTime could work 
    pthread_mutex_init(&mtx_,0) ;
}//* Cron



/** Cron : Default constructor. **/
Scheduler::Cron::Cron(sem_t sem)
{
    //debug(  "C" ) ;
    semap_ = sem ;
    if( cronTab_.empty() ) cronTab_.push_back(RAND_MAX) ; // so that addTime could work 
    pthread_mutex_init(&mtx_,0) ;
}//* Cron



/** Cron : Default destructor. **/
Scheduler::Cron::~Cron()
{
    //debug(  "D" ) ;
    pthread_mutex_destroy( &mtx_ );
}//* ~Cron



void
Scheduler::Cron::addTime( unsigned long timE )
{
    // Insert the element while keeping the list sorted
    list< unsigned long>::iterator it ;
    pthread_mutex_lock(&mtx_);          //-- Lock
    for( it = cronTab_.begin() 
            ; it!= cronTab_.end()
            ; ++it )
        if( timE < *it ) {
            cronTab_.insert( it, timE ) ; 
            pthread_mutex_unlock(&mtx_);//-- Unlock before return
            return;
        }
    pthread_mutex_unlock(&mtx_);        //-- Unlock before function exit
}//* Cron::addTime



void
Scheduler::Cron::delTime(unsigned long timE )
{
    list<unsigned long>::iterator it ;
    pthread_mutex_lock(&mtx_) ;         //-- Lock before we add

    printf("delTime %i\n", timE );
    for( it = cronTab_.begin()
        ; it != cronTab_.end() && *it <= timE 
        ; )
    {
            it = cronTab_.erase( it ) ;
        if( (*it) == timE ) {
            if( it == cronTab_.end() ) return ;
            continue;
        }
        if( it == cronTab_.end() ) return ;
        it++ ;
    }
    pthread_mutex_unlock(&mtx_) ;       //-- Unlock
}//* Cron::delTime



/** runJob : Decrement a semaphore, so a client thread is awaken. **/
    int
Scheduler::Cron::runJob()
{
    //debug(  "B" );

    bool reset ;
    list< unsigned long >::iterator it ;

    printf("+++ENTER JOB++++\n");
    print_time( ) ;
    for( it  = cronTab_.begin( ); it != cronTab_.end(); ++it )
    {
        reset = false ;
        if( !( timeOut_.current() % *it ) )
        {
            reset = true ;          //wtf is this?
            sem_post( &semap_ ) ;   // awake a client( make this a functor ??)
            printf(  "runJob %u\n", *it ) ;
        }
    }
    delTime( timeOut_.current() ) ;
    show() ;

    // If all tasks are called simultaneously, reset curTimeout value:
    if( reset )
        timeOut_.current( timeOut_.min() );
    else
        timeOut_.current( timeOut_.current() + timeOut_.min() ) ;
    printf("---EXIT JOB----\n\n");
    return 0;
}//* Cron::runJob


/** print_time : Debug purpose function. **/
    void
Scheduler::Cron::print_time( )
{
    time_t      rawtime;
    struct tm*  timeinfo;

    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf( "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
    //fprintf( stderr, "Elapsed:%i -- Date: %s", (clock() /CLOCKS_PER_SEC)+1, asctime (timeinfo));
}//* Cron::print_time


void
Scheduler::Cron::show( )
{
    list<unsigned long>::iterator it ;

    it = cronTab_.begin() ;
    for( ; it != cronTab_.end(); ++it )
        cout << *it <<" " ;
    cout <<endl ;
}//* Cron::show



/** refresh : sets the time at which crontab is inspected. **/
    int
Scheduler::Cron::refresh( unsigned long timE )
{
    timeOut_.refresh( timE, cronTab_ ) ;
    return 0 ;
}//* Cron::refresh



/** restart : restart cron. **/
    int
Scheduler::Cron::restart( )
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
Scheduler::Cron::start( )
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
Scheduler::Cron::stop( ) //TODO: timeR could be private
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
Scheduler::Cron::start()
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
Scheduler::Cron::stop()
{
    struct itimerval it;

    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
    it.it_interval = it.it_value;
    return (setitimer(ITIMER_VIRTUAL, &it, NULL));
}
#endif//-------------------FreeBSD TIMERS--------------------------------------



/** TimeOut Default constructor. **/
Scheduler::TimeOut::TimeOut() : min_(0), max_(0), cur_(1)
{
    //debug(  "C" );
}//*TimeOut



/** TimeOut Default destructor. **/
Scheduler::TimeOut::~TimeOut()
{
    //debug(  "D" ) ;
}//* ~TimeOut



/** Refresh the timeList. **/
    void
Scheduler::TimeOut::refresh( int newTimE , list< unsigned long > timeLisT)
{
    //debug(  "B") ;
    if( timeLisT.empty() ) return ;

    max_ = newTimE > max_  ?  newTimE : max_ ;

    list < unsigned long >::iterator it = timeLisT.begin( ) ;
    min_ = timeLisT.front() ;
    for( ; it != timeLisT.end( ) ; ++it )
    {
        unsigned long gcmTime = gcm( min_, *it ) ;
        min_ = min_ > gcmTime ?  gcmTime : min_ ;
        //debug(  "it=%i; gcmTime=%lu, min_=%i", *it, gcmTime, min_ ) ;
    }
    cur_ = min_ ;
    //debug(  "cur_=%i", cur_ ) ;
}//* TimeOut::refresh



/** Greatest Common Divisor. **/
    unsigned long
Scheduler::TimeOut::gcm(unsigned long x, unsigned long y)
{
    //debug(  "B") ;

    if( (x <= 0UL) || (y <= 0UL) || (x==RAND_MAX) || (y==RAND_MAX)) return 1UL ;

    while( x != y )
    {
        if( x > y) x -= y;
        else       y -= x;
    }
    return x;
}//* TimeOut::gcm




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
 * Scheduler::Cron::Start
 * goes through the job queue, and looks
 * at the .timeout memeber.
 * if current_time= .timeout then run
 * the current job callClient() ;
 * inialize the? timer */

