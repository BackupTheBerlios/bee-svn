#include "LoadGen.h"
#include "Producer.hpp"
#include "Consumer.hpp"
#include "tqueue.h"


// -----------------Smtp Load Generator-----------------------------
LoadGen::Smtp::Smtp()
{
}

LoadGen::Smtp::~Smtp()
{
}

// Worker:
// Calculeaza inter-arrival rate-urile
// le pune intr-un timer-queue.
// Timer-ul cand expira inter-arrival-ul, deskide o sessiune catre server
static void* LoadGen::Smtp::worker( void* p )
{
    arrival = rateGen.exponential() ;
}

/**
 * Creez threadurile.
 */
void
LoadGen::Smtp::init()
{
    for( i=0; i < noThreads; ++i )
    {
        pthread_create( &smtpTh[i], 0, routine, arg ) ;
    }
}


/*
 * Fac consumerii */
void
LoadGen::Smtp::run()
{
    for( i=0; i < noThreads; ++i);
    {
        pthread_join( smtpTh[i], 0 );
    }
}


void
LoadGen::Smtp::stop()
{
    running = false ;
    // Dump the results ?
}



// -----------------Pop3 Load Generator-----------------------------
void
LoadGen::Pop3::init()
{
    for( i=0; i < noThreads; ++i )
    {
        pthread_create( &pop3Th[i], 0, routine, arg ) ;
    }
}

void
LoadGen::Pop3::run()
{
    for( i=0; i < noThreads; ++i);
    {
        pthread_join( pop3Th[i], 0 );
    }
}

void
LoadGen::Pop3::stop()
{
    running = false ;
    // Dump the results ?
}
// ---------------- TESTING ---------------------------------

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
        //----SMTP SESSION-------Could be a new function
        smtp.open(host, port);
        smtp.greet() ;
        smtp.mailFrom( "user"+a->userIdx ) ;
        smtp.rcptTo( "user", a->rcptList ); // TODO: RCPT TO: alist of rcpts. "user" is the prefix
        smtp.dataRandom(25*1024) ;
        smtp.quit() ;
        smtp.close() ;
        //----SMTP SESSION-------
        cout <<jobQueue_->size() <<endl ;
        return 0 ;
}//* Consumer::consume



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

