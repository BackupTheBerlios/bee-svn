#define _MULTI_THREADED
#include <iostream>
#include <vector>
#include "prod_cons/Producer.hpp"
#include "prod_cons/Consumer.hpp"
#include "prod_cons/tqueue.h"
#include "distribute/Distribute.h"
#include "scheduler/Scheduler.h"


using namespace std ;


Distribute::Smtp smtpDistr("../distribute/rcpt.dat","../distribute/msgsz.dat") ;
unsigned int elapsed =0 ;

sem_t sched_sem ;
Scheduler::Cron cron( sched_sem ) ;

void tick(union sigval sigval) ;
void* thread_fun(void*) ;



class SmtpConfig {
    public:
        int userIdx ;
        SmtpConfig( ) { userIdx = 10 ; };
};



struct Job {
    int usrIdx ;
    int domIdx ;
    int rcpts ;
    int msg_sz ;
    int rcptList[32] ; // round up to 32
} ;




int run =1 ;

template<class T, class Arg>
T*
Producer<T,Arg>::produce( Arg* a )
{
    printf("++PRODUCE\n" ) ;
        T*      t ;
        t = new T ;
        t->rcpts  = smtpDistr.rcptTo( t->rcptList, 100) ;
        t->usrIdx = smtpDistr.mailFrom(100) ;   // call an exponential distribution here
        t->msg_sz = smtpDistr.msgSize() ;
        return t ;
}//* Producer::produce



template<class T, class Arg>
void
Consumer<T,Arg>::consume( T* a )
{
    Smtp::Protocol smtp ;
        cout <<"--CONSUME:\n"
             <<"--userIdx:" << a->usrIdx <<endl
             <<"--rcpts:"   << a->rcpts <<endl
             <<"--PID: "    <<pthread_self() <<endl;
        cout <<jobQueue_->size() <<endl ;
        smtp.open("localhost", 25);
        smtp.greet("ehlo cucu") ;
        smtp.mailFrom( a->usrIdx ) ;
        smtp.rcptTo( a->rcptList, a->rcpts ) ;
        smtp.dataRandom( a->msg_sz ) ;
        smtp.quit( ) ;
        smtp.close() ;
        //return 0 ;
}//* Consumer::produce




void
smtp_load_gen( int nbcl, int nbtr, int refresh, int tduration ) ;

int main(int argc, char* argv[])
{

    if( argc <=3 ) { printf( "%s clients refresh_time[sec] test_duration[sec]\n", argv[0] ) ;
        return 1 ;
    }
    smtp_load_gen( atoi(argv[1]), 20, atoi(argv[2]), atoi(argv[3])) ;
    return 0 ;
}




void
smtp_load_gen( int nbcl, int nbtr, int refresh, int tduration )
{

    SmtpConfig      smtpConfig ;
    JobQueue<Job*>  jq ;

    sem_t           prod_sem ;
    pthread_t       p ;
    Producer<Job, SmtpConfig>   producer(&smtpConfig) ;

    sem_t           cons_sem ;
    pthread_t       c[nbtr] ;
    Consumer<Job, SmtpConfig>    consumer[nbtr] ;

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

    for( int i=1; i< nbcl ; ++i )
        cron.addTime( i*refresh ) ;   //! addTime parameter is in elapsed.

    cron.refresh( 1*refresh ) ;       //! refresh time
    cron.start() ;                    //! we can start the cron now

    pthread_detach(st);
    while( elapsed <= tduration) 
    {
        ;
    }
    running = 0 ;
    //-----------------CRON------------------


}



void tick(union sigval sigval)
{
    //cron.timer.tick( ) ;
    ++elapsed ;
    cron.runJob();
    cron.addTime( distribute.exponential(mean) ) ;
}

void* thread_fun(void* a)
{
    int* running = (int*) a ; 
    while(*running)
    {
        printf("Running a thread function\n");
        sleep(1);
    }
    return 0 ;
}

