#include "LoadGen.h"
#include "smtp/Protocol.h"

template<class T, class Arg>
T*
Producer<T,Arg>::produce( Arg* a )
{
    Distribute::Smtp* smtpDistr = (Distribute::Smtp*) a ;
    T*      t ;
    t = new T ;
    t->rcpts  = smtpDistr->rcptTo( t->rcptList, 100) ;
    t->usrIdx = smtpDistr->mailFrom(100) ;   // call an exponential distribution here
    t->msg_sz = smtpDistr->msgSize() ;
    printf("++PRODUCE: PID=%i userIdx=%i Qsize=%i\n",pthread_self(), t->msg_sz, jobQueue_->size() ) ;
    return t ;
}//* Producer::produce



template<class T, class Arg>
void
Consumer<T,Arg>::consume( T* a )
{
    printf("--CONSUME: PID=%i userIdx=%i rcpts=%i Qsize=%i\n",
        pthread_self(), a->usrIdx, a->rcpts, jobQueue_->size() ) ;
    sleep(1);
    Smtp::Protocol smtp ;
    try {
        smtp.open("localhost", 25 ) ;
        smtp.greet("ehlo cucu");
        smtp.mailFrom("user");
        smtp.rcptTo("user%i", a->usrIdx, "domain%i", a->usrIdx ) ;
        smtp.quit();
    }catch(Socket::Exception& e )
    {
        printf("ERROR:%s\n", e.what() ) ;
        smtp.close() ;
    }
    //return 0 ;
}//* Consumer::consume




static Scheduler::Cron cron ;
static int running = 1 ;
static int elapsed = 0 ;

// -----------------Smtp Load Generator-----------------------------
LoadGen::Smtp::Smtp( )
{   
    smtpDistr = new Distribute::Smtp("../distribute/rcpt.dat","../distribute/msgsz.dat") ;
    producer.config(smtpDistr) ;
    cron.semaphore( sched_sem ) ;
}

LoadGen::Smtp::~Smtp()
{
    delete smtpDistr ;
}

// Worker:
// Calculeaza inter-arrival rate-urile
// le pune intr-un timer-queue.
// Timer-ul cand expira inter-arrival-ul, deskide o sessiune catre server

void
LoadGen::Smtp::init( int nbcL, int nbtR, int refresH, int tduratioN )
{
    nbcl = nbcL ;
    nbtr = nbtR ;
    refresh = refresH ;
    tduration = tduratioN ;

    /* Init semaphores */
    sem_init( &prod_sem, 0, 0 ) ;
    sem_init( &cons_sem, 0, 0 ) ;
    sem_init( &sched_sem, 0, 0 ) ;
    fprintf( stderr,"DBG:PROD SEM: %p\n" ,&prod_sem ) ;
    //-----------------CONSUMER-------------------------
    // config CONSUMERS to have the intercomunication ways set
    for( int k=0; k< nbtr; ++k)
    {
        consumer[k].jobQueue( &jq ) ;
        consumer[k].prod_sem( &prod_sem ) ;
        //consumer[k].cons_sem( &cons_sem ) ;
        consumer[k].cons_sem( &sched_sem ) ;
    }

    // config PRODUCER to have the intercomunication ways set
    producer.jobQueue( &jq ) ;
    producer.prod_sem( &prod_sem ) ;
    producer.cons_sem( &cons_sem ) ;
}


/*
 * Fac consumerii */
void
LoadGen::Smtp::run()
{
    // CREATE and DETACH producing thread
    pthread_create( &p, 0, producer.execute, &producer ) ;              // producer thread
    pthread_detach( p ) ;

    // CREATE and DETACH consuming threads
    for( int k=0; k<nbtr; ++k)
        pthread_create( &c[k], 0, consumer[k].execute, &consumer[k] ) ; // consumer thread
    for( int k=0; k<nbtr; ++k)
        pthread_detach( c[k] ) ;


    //----------CRON--------
    pthread_t   st ;
    int running = 1 ;
    pthread_create( &st, 0, thread_fun, (void*)&running ) ;
    cron.callback(tick) ;

    printf(" number of clients %i\n", nbcl ) ;
    for( int i=1; i< nbcl ; ++i )
        cron.addTime( i*refresh ) ;   //! addTime parameter is in elapsed.

    cron.refresh( 1*refresh ) ;       //! refresh time
    cron.start() ;                          //! we can start the cron now

    pthread_detach(st);
    while( elapsed <= tduration) 
    {
        ;
    }
    running = 0 ;
    //-----------------CRON------------------
}


void
LoadGen::Smtp::stop()
{
    //running = false ;
    // Dump the results ?
}



void LoadGen::Smtp::tick(union sigval sigval)
{
    //cron.timer.tick( ) ;
    ++elapsed ;
    cron.runJob();
}

void* thread_fun(void* a)
{
    int* running = (int*) a ; 
    while(*running)
    {
        printf("==Thread function\n");
        sleep(1);
    }
    return 0 ;
}

