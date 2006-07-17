#include "LoadGen.h"
#include "smtp/Protocol.h"


static Scheduler::Scheduler cron ;
static int running = 1 ;
static int elapsed = 0 ;

//-----------------Smtp Load Generator-----------------------------
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
    sem_ = cron.semaphore() ;
}//* LoadGen::Smtp::init--------------------------------------------



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
        //sleep(1);
    }
    running = 0 ;
}//* LoadGen::Smtp::run---------------------------------------------


/**
 * Comment. */
void
LoadGen::Smtp::stop()
{
    //running = false ;
    // Dump the results ?
}//* LoadGen::Smtp::stop---------------------------------------------


























//-----------------Pop3 Load Generator-----------------------------
LoadGen::Pop3::Smtp( )
{
    smtpDistr = new Distribute::Pop3("./data/rcpt.csv","./data/msgsz.csv") ;
}


LoadGen::Pop3::~Smtp()
{
    delete smtpDistr ;
}


// static threaded function
void*
LoadGen::Pop3::worker( void* a )
{
    config_t* p = (config_t*) a ;
    ::Pop3::Protocol smtp ;

    while( 1 ) 
    {
        sem_wait( ((LoadGen::Pop3*)p->ths)->sem_ ) ;
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
LoadGen::Pop3::init( config* cfg )
{
    cfg_ = cfg ;
    cfg_->ths = this ;
    sem_ = cron.semaphore() ;
}//* LoadGen::Pop3::init--------------------------------------------



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
        //sleep(1);
    }
    running = 0 ;
}//* LoadGen::Pop3::run---------------------------------------------


/**
 * Comment. */
void
LoadGen::Pop3::stop()
{
    //running = false ;
    // Dump the results ?
}//* LoadGen::Pop3::stop---------------------------------------------

