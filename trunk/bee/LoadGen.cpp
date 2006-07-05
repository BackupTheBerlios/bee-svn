#include "LoadGen.h"



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
