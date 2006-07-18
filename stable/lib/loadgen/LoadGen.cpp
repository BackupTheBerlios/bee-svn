#include "LoadGen.h"
#include "smtp/Protocol.h"


LoadGen::Smtp::Smtp( )
{
    smtpDistr = new Distribute::Smtp("./data/rcpt.csv","./data/msgsz.csv") ;
}


LoadGen::Smtp::~Smtp()
{
    delete smtpDistr ;
}


// static threaded function
void*
LoadGen::Smtp::worker( void* a )
{
    config_t* p = (config_t*) a ;
    ::Smtp::Protocol smtp ;

    while( 1 ) 
    {
        sem_wait( ((LoadGen::Smtp*)p->ths)->sem_ ) ;
        printf("pthread_self %u\n\n\n", pthread_self() ) ;
        int rcpts  = 1;//smtpDistr->rcptTo( t->rcptList, 100) ;
        int usrIdx = 1;//smtpDistr->mailFrom(100) ;   // call an exponential distribution here
        int msg_sz = 1;//smtpDistr->msgSize() ;
        try {

            printf("host:%s port:%i\n", p->smtp_server, p->smtp_port );
            smtp.open( p->dest ) ;
            smtp.greet("ehlo cucu");
            smtp.mailFrom("user");
            smtp.rcptTo("user%i", usrIdx, "domain%i", usrIdx ) ;
            smtp.rcptTo( "user1"/*rcpts, rcptList*/ ) ;
            smtp.randomData(msg_sz) ;
            smtp.quit();
        }catch(Socket::Exception& e )
        {
            printf("ERROR:%s\n", e.what() ) ;
            smtp.close() ;
        }
    }
}





void
LoadGen::Smtp::init( config* cfg )
{
    cfg_ = cfg ;
    cfg_->ths = this ;
    sem_ = cron.semaphore() ; // Cron will init his semaphore by itself
}



/*
 * Fac consumerii */
void
LoadGen::Smtp::run()
{
    pthread_t c[30] ;

    // CREATE and DETACH consuming threads
    for( int k=0; k<20; ++k)
        pthread_create( &c[k], 0, worker, (void*)cfg_ ) ; // consumer thread
    for( int k=0; k<20; ++k)
        pthread_detach( c[k] ) ;

    timeval tv ;
    gettimeofday(&tv,0);
    while( cron.elapsed() <= tv.tv_sec+cfg_->span )
    {
        cron.run();
    }
    running = 0 ;
}



/**
 * Comment. */
void
LoadGen::Smtp::stop()
{
    //running = false ;
    // Dump the results ?
}


























//-----------------Pop3 Load Generator-----------------------------
LoadGen::Pop3::Smtp( )
{
    // will have to implement Distribute::pop3
    pop3Distr = new Distribute::Pop3("./data/rcpt.csv","./data/msgsz.csv") ;
}


LoadGen::Pop3::~Smtp()
{
    delete pop3Distr ;
}


// static threaded function
void*
LoadGen::Pop3::worker( void* a )
{
    config_t* p = (config_t*) a ;
    ::Pop3::Protocol pop3 ;
    float p ;

    while( 1 )
    {
        sem_wait( ((LoadGen::Pop3*)p->ths)->sem_ ) ;
        printf("pthread_self %u\n\n\n", pthread_self() ) ;

        p = erand48(xsubi_);// pick this randomly, and use a CMF
        if( p<0.075 )
            user = d.retry_pool() ;
        else
            user = d.random_pool() ;

        try {
            printf("host:%s port:%i\n", p->smtp_server, p->smtp_port );
            pop3.open( p->dest ) ;
            pop3.login( user, user ) ;
            pop3.stat() ;
            pop3.retr() ;
            pop3.dele() ;
            pop3.quit() ;
        }catch(Socket::Exception& e )
        {
            printf("ERROR:%s\n", e.what() ) ;
            pop3.close() ;  // dont left the socket oppened
        }
    }
}





void
LoadGen::Pop3::init( config* cfg )
{
    cfg_ = cfg ;
    cfg_->ths = this ;
    sem_ = cron.semaphore() ;
}



/*
 * Fac consumerii */
void
LoadGen::Pop3::run()
{
    pthread_t c[30] ;

    // CREATE and DETACH consuming threads
    for( int k=0; k<20; ++k)
        pthread_create( &c[k], 0, worker, (void*)cfg_ ) ; // consumer thread
    for( int k=0; k<20; ++k)
        pthread_detach( c[k] ) ;

    timeval tv ;
    gettimeofday(&tv,0);
    while( cron.elapsed() <= tv.tv_sec+cfg_->span )
    {
        cron.run();
    }
    running = 0 ;
}


/**
 * Comment. */
void
LoadGen::Pop3::stop()
{
    // running = false ;
    // Dump the results ?
}

