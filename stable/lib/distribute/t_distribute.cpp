#include "Distribute.h"


int
main(int argc, char* argv[] )
{

    if( argc < 3 ) { printf("%s nb_picks maxRcpt\n", argv[0] ) ; return 2 ; }

    Distribute::Smtp smtpDistr( "./rcpt.csv", "./msgsz.csv" );
    int nb_tries = atoi( argv[1] ) ;
    int maxRcpt = atoi(argv[2] ) ;
    int rcptList[30] ;

    printf("----Testing Distribute::Smtp::rcptTo\n");
    
    for( int i=0; i< nb_tries; ++i )
    {
        int nb_rcpt = smtpDistr.rcptTo(rcptList,maxRcpt) ;
        printf("Will send to : ");
        for( int k=0; k< nb_rcpt; ++k )
            printf( " %i,", rcptList[k] ) ;
        printf("\n");
    }
    printf("----Testing Distribute::Smtp::msgSize\n");
    for( int i=0; i< nb_tries; ++i )
    {
        int sz = smtpDistr.msgSize() ;
        printf("send =%i=\t[kilo-bytes]\n", sz );
    }
    
    printf("----Testing Distribute::Smtp::mailFrom\n");
    for( int i=0; i< nb_tries; ++i )
    {
        int sz = smtpDistr.mailFrom(maxRcpt) ;
        printf("mailForm =%i=\n", sz );
    }
    return 0 ;
}
