#define _MULTI_THREADED
#include <iostream>
#include <vector>
#include "LoadGen.h"

using namespace std ;

int main(int argc, char* argv[])
{

    if( argc <=3 ) { printf( "%s clients refreshTime[sec] duration[sec]\n", argv[0] ) ;
        return 1 ;
    }
    LoadGen::Smtp smtpGen ;
    smtpGen.init( atoi(argv[1]), 20, atoi(argv[2]), atoi(argv[3])) ;
    smtpGen.run( ) ;
    return 0 ;
}



