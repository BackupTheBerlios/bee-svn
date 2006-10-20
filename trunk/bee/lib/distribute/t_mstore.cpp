#include "MailStore.h"

int
main() {
    MailStore ms("./data/md.csv") ;
    ms.init("localhost", 25, 10000,4 ) ;
    return 0;
}

