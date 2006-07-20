#include "LoadGen.h"
#include "smtp/Protocol.h"
#include "pop3/Protocol.h"


LoadGen::Smtp::Smtp( )
{
    smtpDistr = new Distribute::Smtp("./data/rcpt.csv","./data/msgsz.csv") ;
}


LoadGen::Smtp::~Smtp()
{
    delete smtpDistr ;
}




void
LoadGen::Smtp::init( config* cfg )
{
    cfg_    = cfg ;
    sem_    = cron.semaphore() ; // Cron will init his semaphore by itself
    cfg_->ths = this ;
}



void
LoadGen::Smtp::run()
{
    pthread_t c[cfg_->threads] ;
    int rc ;

    // CREATE and DETACH consuming threads
    for( int k=0; k<cfg_->threads; ++k)
    {
        rc = pthread_create( &c[k], 0, worker, (void*)cfg_ ) ;
        if( rc == 0 ) pthread_detach( c[k] ) ;
        else printf( "Error creating thread %i\n", k ) ;
    }

    timeval tv ;
    gettimeofday(&tv,0);
    while( cron.elapsed() <= tv.tv_sec+cfg_->span )
    {
        cron.run();
    }
}



// static threaded function
void*
LoadGen::Smtp::worker( void* a )
{
    config_t* p = (config_t*) a ;
    ::Smtp::Protocol smtp ;
    LoadGen::Smtp* ths = (LoadGen::Smtp*) p->ths ;
    int rcptList[32] ;

    while( 1 )
    {
        sem_wait( ths->sem_ ) ;
        printf("pthread_self %u\n\n\n", pthread_self() ) ;
        int rcpts  = ths->smtpDistr->rcptTo( rcptList, p->users) ;
        int usrIdx = ths->smtpDistr->mailFrom(p->users) ;          // call an exponential distribution here
        int msg_sz = ths->smtpDistr->msgSize() ;
        try
        {   printf("host:%s port:%i\n", p->smtp_server, p->smtp_port );
            smtp.open( p->dest ) ;
            smtp.greet("ehlo cucu");
            smtp.mailFrom("user"); // TODO does specmail says smth abt this ?
            // if() { local_user() } else {remote_user() }
            smtp.rcptTo( rcpts, rcptList ) ; // TODO add domain parameter
            smtp.randomData(msg_sz) ;
            smtp.quit();
        }catch(Socket::Exception& e )
        {
            printf("ERROR reported %s\n", e.what() ) ;
            smtp.close() ;
        }
    }
}





/**
 * Comment. */
void
LoadGen::Smtp::stop()
{
    //running = false ;
    // Dump the results ?
}







    throw ReportEx("popen");






















//-----------------Pop3 Load Generator-----------------------------
LoadGen::Pop3::Pop3( )
{
    // will have to implement Distribute::pop3
    pop3Distr = new Distribute::Pop3(cfg_) ;
    xsubi_[0] = (unsigned short)(0x1234 ^ 12);
    xsubi_[1] = (unsigned short)(0x5678 ^ (12 << 8));
    xsubi_[2] = (unsigned short)(0x9abc ^ ~12);
}


LoadGen::Pop3::~Pop3()
{
    delete pop3Distr ;
}




void
LoadGen::Pop3::init( config_t* cfg )
{
    cfg_    = cfg ;
    sem_    = cron.semaphore() ;
    cfg_->pths = this ;
}



/*
 * Fac consumerii */
void
LoadGen::Pop3::run()
{
    pthread_t c[cfg_->threads] ;
    int rc ;

    // CREATE and DETACH consuming threads
    for( int k=0; k<cfg_->threads; ++k)
    {
        rc = pthread_create( &c[k], 0, worker, (void*)cfg_ ) ;
        if( rc == 0 ) pthread_detach( c[k] ) ;
        else printf("Error creating thread %i\n", k ) ;
    }
    timeval tv ;
    gettimeofday(&tv,0);
    while( cron.elapsed() <= tv.tv_sec+cfg_->span )
    {
        cron.run();
    }
}



// static threaded function
void*
LoadGen::Pop3::worker( void* a )
{
    config_t* p = (config_t*) a ;
    LoadGen::Pop3* ths = (LoadGen::Pop3*) p->pths ;
    ::Pop3::Protocol pop3 ;
    float pct ;
    int user ;
    int m, s ;

    while( 1 )
    {
        sem_wait( ths->sem_ ) ;
        printf("pthread_self %u\n\n\n", pthread_self() ) ;

        pct = erand48( ths->xsubi_);
        if( pct<0.075 )
            user = ths->pop3Distr->retry_user() ;
        else
            user = ths->pop3Distr->random_user() ;

        try {
            printf("host:%s port:%i\n", p->smtp_server, p->smtp_port );
            pop3.open( p->pdest ) ;
            pop3.user( p->user_prefix, user ) ;
            pop3.pass( p->user_prefix, user ) ;
            pop3.stat(&m, &s) ;
            printf("MESSAGES:%i\n", m) ;
            while(m--) {
                pop3.retr(m) ;
                pop3.dele(m) ;
            }
            pop3.quit() ;
        }catch(Socket::Exception& e )
        {
            printf("ERROR reported:%s\n", e.what() ) ;
            pop3.close() ;  // dont left the socket open
        }catch(ReportEx& e )
        {
            printf("POP3 ERROR %s\n", e.what ) ;
            pop3.quit() ;
        }

    }
}



/**
 * Comment. */
void
LoadGen::Pop3::stop()
{
    // running = false ;
    // Dump the results ?
}

