#include "LoadGen.h"

LoadGen::LoadGen()
{
}

LoadGen::~LoadGen()
{
}

/**
 * Creez threadurile.
 */
void
LoadGen::initSmtp()
{
    for(i=0; i<nth; ++i )
    {
        pthread_create() ;
    }
}

RateGen rateGen(mean);

static void* LoadGen::foo( void* p )
{
    arrival = rateGen.exponential() ;
}

void
LoadGen::runSmtp()
{
    for( i=0; i< nth; ++i);
    {
        pthread_join( ta[i], foo, arg,0 );
    }
}


void
LoadGen::stopSmtp()
{
    running = false ;
}


void
LoadGen::initPop3()
{
}

void
LoadGen::stopPop3()
{
}

void
LoadGen::runPop3()
{
}
