#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
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

struct option long_options[] =
{
    {"port", 1, 0, 0},
    {"threads", 0, 0, 0},
    {"initonly", no_argument,       &iflag, 1},
    {"clients", 0, 0, 0},
    {"resolution", 1, 0, 0},
    {"span", 1, 0, 0},
    {0, 0, 0, 0}
};

    opterr = 0;
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }
    int option_index = 0;

    while( -1 != (c = getopt_long_only( argc, argv, "ihH:P:T:C:R:S:U:", long_options, &option_index)) )
    {
        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
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
        ms.init( host, port, users, nbThr ) ;
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
    printf("\t-initonly\t\tInit storage\n" ) ;
    printf("\t-host host\t\tserver's Host\n" ) ;
    printf("\t-port port\t\tserver's Port\n" ) ;
    printf("\t-threads threads\tNumber of threads used for[gotta think of]\n" );
    printf("\t-clients clients\tNumber of clients\n" ) ;
    printf("\t-resolution sec\t\tResolution time\n" ) ;
    printf("\t-span sec\t\ttest Span in seconds\n" ) ;
    printf("\t-users users\tNumber of users on server\n" );

    printf("\t-h\t\tThis message\n" ) ;
    return 0 ;
}

