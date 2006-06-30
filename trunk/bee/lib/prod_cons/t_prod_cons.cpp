#define _MULTI_THREADED
#include <iostream>
#include <vector>
#include "Producer.hpp"
#include "Consumer.hpp"
#include "tqueue.h"

using namespace std ;

class SmtpTest {
    public:
    int userIdx ;
    SmtpTest( )
    {
        userIdx = 10 ;
    };
};


int run =1 ;

template<class T, class Arg>
T*
Producer<T,Arg>::produce( Arg* a )
{
    printf("++PRODUCE\n" ) ;
        T*      t ;
        t = new T ;
        *t = 1 ;
        return t ;
}//* Producer::produce



template<class T, class Arg>
T*
Consumer<T,Arg>::consume( Arg* a )
{
        cout <<"--CONSUME:" <<a <<endl
             <<"--PID: " <<pthread_self() <<endl;
        sleep(1);
        cout <<jobQueue_->size() <<endl ;
        return 0 ;
}//* Consumer::produce



int main()
{
    SmtpTest smtpTest ;
    JobQueue<int*>   jq ;

    sem_t prod_sem ;
    pthread_t   p ;
    Producer<int, SmtpTest>   prod(&smtpTest) ;

    sem_t cons_sem ;
    pthread_t   c[20] ;
    Consumer<int, SmtpTest>    cons[20] ;
    
    int k ;
    for( k=0; k<20; ++k)
    {
        //cons[k].setConf( &smtpTest ) ;
        cons[k].jobQueue( &jq ) ;
        cons[k].prod_sem( &prod_sem ) ;
        cons[k].cons_sem( &cons_sem ) ;
    }


    sem_init( &prod_sem, 0, 0 ) ;
    sem_init( &cons_sem, 0, 0 ) ;
    cout <<"PROD SEM: " <<&prod_sem <<endl ;

    prod.jobQueue( &jq ) ;

    // Init step 2 : Set condition variables.
    prod.prod_sem( &prod_sem ) ;
    prod.cons_sem( &cons_sem ) ;


    pthread_create( &p, 0, prod.execute, &prod ) ; // producer thread
    for( k=0; k<20; ++k)
        pthread_create( &c[k], 0, cons[k].execute, &cons[k] ) ; // consumer thread

    pthread_join( p, 0 ) ;
    for( k=0; k<20; ++k)
        pthread_join( c[k], 0) ;
    return 0 ;
}
