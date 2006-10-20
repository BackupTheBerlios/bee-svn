#include "RateGen.h"
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>

int main(int argc, char* argv[] )
{
    if( argc < 3 ) { printf("%s picks frequency\n", argv[0] ) ; exit(2) ; }
    timeval tv ;
    gettimeofday( &tv, 0 );
    
    RateGen r( tv.tv_usec, atoi(argv[2]) ) ;

    for(int i=0; i<atoi(argv[1]); ++i)
        printf("%f\n", r.exponential() ) ;

    return 0 ;
}
