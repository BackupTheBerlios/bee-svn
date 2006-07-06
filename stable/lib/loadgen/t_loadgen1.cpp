#include "LoadGen.h"



int main() 
{
    LoadGen::Smtp smtpGen ;
    smtpGen.init() ;
    smtpGen.stop() ;
    return 0 ;
}
