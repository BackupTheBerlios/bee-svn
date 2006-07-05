#include "LoadGen.h"

LoadGen::LoadGen()
{
}

LoadGen::~LoadGen()
{
}

// Worker:
// Calculeaza inter-arrival rate-urile
// le pune intr-un timer-queue.
// Timer-ul cand expira inter-arrival-ul, deskide o sessiune catre server
static void* LoadGen::worker( void* p )
{
    arrival = rateGen.exponential() ;
}

/**
 * Creez threadurile.
 */
void
LoadGen::initSmtp()
{
    for( i=0; i < numThreads; ++i )
    {
        pthread_create( &smtpTh[i], 0, routine, arg ) ;
    }
}

void
LoadGen::runSmtp()
{
    for( i=0; i < numThreads; ++i);
    {
        pthread_join( smtpTh[i], 0 );
    }
}


void
LoadGen::stopSmtp()
{
    running = false ;
    // Dump the results ?
}


void
LoadGen::initPop3()
{
    for( i=0; i < numThreads; ++i )
    {
        pthread_create( &pop3Th[i], 0, routine, arg ) ;
    }
}

void
LoadGen::stopPop3()
{
    running = false ;
    // Dump the results ?
}

void
LoadGen::runPop3()
{
    for( i=0; i < numThreads; ++i);
    {
        pthread_join( pop3Th[i], 0 );
    }
}
