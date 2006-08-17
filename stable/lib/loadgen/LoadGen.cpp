#include "LoadGen.h"
#include "smtp/Protocol.h"
#include "pop3/Protocol.h"
#include <cmath>


LoadGen::Smtp::Smtp( )
{
    smtpDistr = new Distribute::Smtp("./data/rcpt.csv","./data/msgsz.csv") ;
    xsubi_[0] = (unsigned short)(0x1234 ^ 12);
    xsubi_[1] = (unsigned short)(0x5678 ^ (12 << 8));
    xsubi_[2] = (unsigned short)(0x9abc ^ ~12);
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
    rcpt_t rcptList[32] ;
    bool modem ;


    while( 1 )
    {
        sem_wait( ths->sem_ ) ;
        debug("pthread_self %u", pthread_self() ) ;
        int rcpts  = ths->smtpDistr->rcptTo( rcptList, p->users) ;
        int usrIdx = ths->smtpDistr->mailFrom(p->users) ;          // call an exponential distribution here
        int msg_sz = ths->smtpDistr->msgSize() ;
        float speed = (1.0*rand())/RAND_MAX ;
        modem = false ;
        if( speed <= 0.421 )
            modem = true ;

        try
        {
            int sec=0 ;
            float sec_tout=0.0 ;
            smtp.timeout(5) ; //6,000
            smtp.latency(modem) ;
            //smtp.open( "gigi", 25 ) ;
            smtp.open( p->dest ) ;
            smtp.greet("ehlo cucu");
            smtp.mailFrom("<>");                // TODO does specmail says smth abt this ?
            smtp.rcptTo( rcpts, rcptList ) ;    // TODO add domain parameter
            // QoS is different for DATA
            sec_tout= 5 + ((0.1*msg_sz) / 3000  ) ;
            sec     = (int)floor(sec_tout) ;
            smtp.timeout(sec, ((int)sec_tout-sec)*1000000) ; // 1,000,000 * usec
            smtp.randomData(msg_sz) ;
            // Default QoS
            smtp.timeout(5) ; //6,000
            smtp.quit();
        }catch(Socket::Exception& e )
        {
            fprintf( stderr, "SMTP SocketError :%s\n", e.what() ) ;
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
        debug("pthread_self %u", pthread_self() ) ;

        pct = (1.0*rand())/RAND_MAX ;
        if( pct<0.075 )
            user = ths->pop3Distr->retry_user() ;
        else
            user = ths->pop3Distr->random_user() ;

        try {
            debug("host:%s port:%i\n", p->smtp_server, p->smtp_port );
            //pop3.open( "gigi", 110 ) ;
            pop3.open( p->pdest ) ;
            pop3.user( p->user_prefix, user ) ;
            pop3.pass( p->user_prefix, user ) ;
            pop3.stat(&m, &s) ;
            debug("MESSAGES:%i SIZE%i\n", m,s ) ;
            while(m--) {
                int   msg_sz = pop3.list(m) ;
                float sec_tout = 0.0 ;
                int   sec = 0 ;
                // QoS is different for RETR
                sec_tout= 5 + ((0.1*msg_sz) / 3000  ) ;
                sec     = (int)floor(sec_tout) ;
                pop3.timeout(sec, ((int)sec_tout-sec)*1000000) ; // 1,000,000 * usec
                pop3.retr(m) ;
                // Default QoS
                pop3.timeout(5) ;
                pop3.dele(m) ;
            }
            pop3.quit() ;
        }catch(Socket::Exception& e )
        {
            fprintf( stderr, "POP3 SocketError :%s\n", e.what() ) ;
            pop3.close() ;  // dont left the socket open
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

