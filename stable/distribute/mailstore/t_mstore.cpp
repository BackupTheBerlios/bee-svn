#include "MailStore.h"

int
main() {
    MailStore ms("./md.dat") ;
    ms.init("localhost", 25, 10000,4 ) ;
    return 0;
}

