#include <unistd.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include "distribute/MailStore.h"
#include "loadgen/LoadGen.h"


//Benchmark bench ;
int usage( const char* prog ) ;


    int
main (int argc, char **argv)
{
    int     iflag=0, port=0, users=0, idx=0, c=0, nbThr=1, nbClt=1,res=1 ;
    int     tSpan=0 ;
    char*   host=0 ;
    bool    is_filled=false ;

    opterr = 0;
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }

    while( -1 != (c = getopt( argc, argv, "ihc:H:P:T:C:R:S:U:")) )
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
                host  = optarg;
                break;
            case 'P':
                port  = atoi(optarg) ;
                break;
            case 'U':
                users = atoi(optarg) ;
                break;
            case 'T':
                nbThr = atoi(optarg) ;
                break;
            case 'C':
                nbClt = atoi(optarg) ;
                break;
            case 'R':
                res = atoi(optarg) ;
                break;
            case 'S':
                tSpan = atoi(optarg) ;
                break;
            case ':':   // -H -P -U -T -C without operand
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
        if(!users||!port){
            printf("Use -U parameter to specify the number of users\n");
            printf("and -P parameter to specify the SMTP port number\n");
            exit(2);
        }
        MailStore ms("./data/md.csv") ;
        ms.init( host, port, users, 10 ) ;    // HARDCODED:Numarul de threaduri folosit la populare
        is_filled = true ;
        exit(0);
    }
    LoadGen::Smtp smtpGen ;
    smtpGen.init( nbClt, nbThr, res, tSpan ) ;
    smtpGen.run( ) ;

/*
    bench.cron.callback(tick) ;
    bench.run( 80,  is_filled ) ;
    bench.run( 100, is_filled ) ;
    bench.run( 120, is_filled ) ;
*/
    return 0;
}

/*
// cron trb sa fie o variabila globală( that sux )
void tick(union sigval sigval)
{
    bench.cron.timer.tick() ; //! increase the value of elapsed( ++elapsed)
    bench.cron.runJob();
}
*/


int
usage(const char* app)
{
    printf("Usage %s [OPTIONS]\n", app );
    printf("\t-i\t\tInit storage\n" ) ;
    printf("\t-H host\t\tserver's Host\n" ) ;
    printf("\t-P port\t\tserver's Port\n" ) ;
    printf("\t-T threads\tNumber of threads used for[gotta think of]\n" );
    printf("\t-C clients\tNumber of clients\n" ) ;
    printf("\t-R sec\t\tResolution time\n" ) ;
    printf("\t-S sec\t\ttest Span in seconds\n" ) ;
    printf("\t-U users\tNumber of users on server\n" );

    printf("\t-h\t\tThis message\n" ) ;
    return 0 ;
}

