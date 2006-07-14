#include "RateGen.h"
#include <cstdlib>
#include <math.h>


RateGen::RateGen( int seed, double freq )
{
    xsubi_[0] = 0x1234 ^ seed;
    xsubi_[1] = 0x5678 ^ (seed << 8);
    xsubi_[2] = 0x9abc ^ ~seed;
    freq_     = freq ;
    mean_     = 1/freq_ ;
}



double
RateGen::uniform( double upper, double lower ) 
{
    return lower + (upper - lower)*erand48( xsubi_ ) ;
}

double
RateGen::exponential() 
{
    return -mean_*log( 1.0 - erand48(xsubi_) ) ;
}
