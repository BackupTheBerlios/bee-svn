#define _MULTI_THREADED
#include <iostream>
#include <vector>
#include "Producer.hpp"
#include "Consumer.hpp"
#include "tqueue.h"

using namespace std ;

class SmtpConfig {
    public:
        int userIdx ;
        SmtpConfig( ) { userIdx = 10 ; };
};



struct Job {
    int usrIdx ;
    int domIdx ;
    int rcpts ;
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
        t->rcpts  = 1;//smtpDistr.rcptTo( &t->rcptList) ;
        t->usrIdx = 1;//smtpDistr.mailFrom() ;   // call an exponential distribution here
        //t->msg_sz = 1;//smtpDistr.msgSize() ;
        return t ;
}//* Producer::produce



template<class T, class Arg>
void
Consumer<T,Arg>::consume( T* a )
{
        cout <<"--CONSUME:\n"
             <<"--userIdx:"   << a->userIdx <<endl
             <<"--rcpts:"   << a->rcpts <<endl 
             <<"--PID: " <<pthread_self() <<endl;
        sleep(1);
        cout <<jobQueue_->size() <<endl ;
        //return 0 ;
}//* Consumer::produce



int main()
{
    SmtpConfig smtpConfig ;
    JobQueue<Job*>   jq ;

    pthread_t   p ;
    Producer<Job, SmtpConfig>   prod(&smtpConfig) ;

    sem_t cons_sem ;
    pthread_t   c[20] ;
    Consumer<Job, SmtpConfig>    cons[20] ;
    
    sem_init( &cons_sem, 0, 0 ) ;
    cout <<"PROD SEM: " <<&prod_sem <<endl ;

    prod.set_ipc( &jq, &cons.semaphore() ) ;
    for( int k=0; k<20; ++k)
    {
        cons[k].set_ipc( &jq, prod.semaphore() ) ;
    }

    pthread_create( &p, 0, prod.execute, &prod ) ;              // producer thread
    for( k=0; k<20; ++k)
        pthread_create( &c[k], 0, cons[k].execute, &cons[k] ) ; // consumer thread

    pthread_join( p, 0 ) ;
    for( k=0; k<20; ++k)
        pthread_join( c[k], 0) ;
    return 0 ;
}
