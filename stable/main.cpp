#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include "distribute/mailstore/MailStore.h"


Benchmark bench ;
int usage( const char* prog ) ;


    int
main (int argc, char **argv)
{
    int     iflag=0, port=25, users=0, idx=0, c=0 ;
    char*   host=0 ;
    bool    is_filled=false ;

    opterr = 0;
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }

    while( -1 != (c = getopt( argc, argv, "ihc:H:P:")) )
    {
        switch (c)
        {
            case 'i':
                iflag = 1;
                break;
            case 'h':
                usage(argv[0]) ;
                exit(0) ;
            case 'H':
                host = optarg;
                break;
            case 'P':
                port = atoi(optarg);
                break;
            case 'U':
                users = atoi(optarg);
                break;
            case ':':   // -H -P -U without operand
                fprintf( stderr, "Option %c requires a parameter.\n", optarg ) ;
                usage(argv[0]) ;
                exit(2) ;
            case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                usage(argv[0]) ;
                exit(2) ;
            default:
                break ;
        }
    }

    for( idx=optind; idx < argc; idx++) printf ("Non-argument %s\n", argv[idx]);

    // Pre-populez `users` mailBoxuri pe host:port folosind 10 threaduri
    if( iflag )
    {
        MailStore ms("./distribute/mailstore/md.dat") ;
        ms.init( host, port, users, 10 ) ;    // HARDCODED:Numarul de threaduri folosit la populare
        is_filled = true ;
    }
    bench.cron.callback(tick) ;
    bench.run( 80,  is_filled ) ;
    bench.run( 100, is_filled ) ;
    bench.run( 120, is_filled ) ;

    return 0;
}


// cron trb sa fie o variabila globală( that sux )
void tick(union sigval sigval)
{
    bench.cron.timer.tick() ; //! increase the value of elapsed( ++elapsed)
    bench.cron.runJob();
}



int
usage(const char* app)
{
    printf("Usage %s [OPTIONS]\n", app );
    printf("\t-i\tInit storage\n" ) ;
    printf("\t-H host\tserver's Host\n" ) ;
    printf("\t-P port\tserver's Port\n" ) ;
    printf("\t-U users\tNumber of users on server\n" );
    printf("\t-h\tThis message\n" ) ;
    return 0 ;
}

