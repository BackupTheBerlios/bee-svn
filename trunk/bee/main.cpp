#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include "config.h"
#include "distribute/mailstore/MailStore.h"

int
usage(const char* app)
{
    printf("Usage %s [OPTIONS]\n", app );
    printf("\t-i\tInit storage\n" ) ;
    return 0 ;
}


    int
main (int argc, char **argv)
{
    int iflag = 0;
    int hflag = 0;
    char *cvalue = NULL;
    int index;
    int c;
    bool mstore_state = false ;

    opterr = 0;
    if( argc < 2 ) { usage(argv[0] ); exit(1) ; }
    while ((c = getopt (argc, argv, "ihc:")) != -1)
        switch (c)
        {
            case 'i':
                iflag = 1;
                break;
            case 'h':
                hflag = 1;
                break;
            case 'c':
                cvalue = optarg;
                break;
            case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                break ;
        }
    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    if( hflag ) { usage(argv[0]) ; return 0 ; }
    //
    // Initialize 10,000 mailBoxes on localhost:25 using 4 threads
    if( iflag )
    {
        MailStore ms("./distribute/mailstore/md.dat") ;
        ms.init( SMTP_SERVER ,SMTP_PORT, USER_END-USER_START + 1 ,10 ) ;
        mstore_state = true ;
    }

    // a SPECmail2001 benchmark run consists of a series of
    // warm-up,
    // measurement for 80%, 100% and 120% load 
    // and cool-down phases:
    benchmark.run( 80,  mstore_state ) ;
//    benchmark.run( 100, mstore_state ) ;
//    benchmark.run( 120, mstore_state ) ;

    return 0;
}
