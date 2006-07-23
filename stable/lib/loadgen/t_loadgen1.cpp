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
    config_t cfg ;
    smtpGen.init( &cfg ) ;
    smtpGen.run( ) ;
    return 0 ;
}



