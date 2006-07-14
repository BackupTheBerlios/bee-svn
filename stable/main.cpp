#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include "distribute/MailStore.h"
#include "loadgen/LoadGen.h"


//Benchmark bench ;
int usage( const char* prog ) ;
//test

int
usage(const char* app)
{
    printf("Usage %s [OPTIONS]\n", app );
    printf("\t-h\t\t\tHelp message\n" ) ;
    printf("\t-initonly\t\tInit storage\n" ) ;
    printf("\t-port port\t\tserver's Port\n" ) ;
    printf("\t-threads threads\tNumber of threads used for[gotta think of]\n" );
    printf("\t-clients clients\tNumber of clients\n" ) ;
    printf("\t-resolution sec\t\tResolution time\n" ) ;
    printf("\t-span sec\t\ttest Span in seconds\n" ) ;
    printf("\t-users users\t\tNumber of users on server\n" );
    printf("\t-smtpserver\t\tThe host name of the SMTP server\n");
    printf("\t-smtpport\t\tThe host port number of the SMTP server\n");
    printf("\t-pop3server\t\tThe host name of the POP3 server\n");
    printf("\t-pop3port\t\tThe host port number of the POP3 server\n");
    printf("\t-sinkserver\t\tThe host name of the SINK server\n");
    printf("\t-sinkport\t\tThe host port number of the SINK server\n");
    printf("\t-localdomain\t\tThe domain name that represents local mail on the mail server\n");
    printf("\t-remotedomain\t\tThe domain name that represents remote mail on the mail server\n");
    printf("\t-userprefix\t\tThe username has the form USERPREFIX_userid\n") ;
    printf("\t-userpasswd\t\tThe username password\n") ;
    printf("\t-userstart\t\tThe starting number of userid\n") ;
    printf("\t-userend\t\tThe ending number of userid\n") ;
    printf("\t-initrest\t\tHow many minutes to wait after completing mailbox initialization\n") ;

    return 0 ;
}
    int
main (int argc, char **argv)
{
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }

    int     iflag=0, port=0, users=0, idx=0, c=0, nbThr=1, nbClt=1,res=1 ;
    int     tSpan=0 ;
    char*   host=0 ;
    bool    is_filled=false ;
    int     option_index = 0;

    struct option long_options[] =
    {
        {"initonly"     , 0, &iflag, 1},
        {"smtpserver"   , 1, 0, 2},
        {"smtpport"     , 1, 0, 3},
        {"pop3server"   , 1, 0, 4},
        {"pop3port"     , 1, 0, 5},
        {"sinkserver"   , 1, 0, 6},
        {"sinkport"     , 1, 0, 7},
        {"localdomain"  , 1, 0, 8},
        {"remotedomain" , 1, 0, 9},
        {"userprefix"   , 1, 0,10},
        {"userpasswd"   , 1, 0,11},
        {"userstart"    , 1, 0,12},
        {"userend"      , 1, 0,13},
        {"initrest"     , 1, 0,14},
        {"threads"      , 1, 0,15},
        {"clients"      , 1, 0,16},
        {"span"         , 1, 0,17},
        {"warmup"       , 1, 0,18},
        {"rampdown"     , 1, 0,19},
        {"users"        , 1, 0,20},
        {"resolution"   , 1, 0,21},

        {0, 0, 0, 0}
    };

    opterr = 0;

    while( -1 != (c = getopt_long_only( argc, argv, "ihH:P:T:C:R:S:U:", long_options, &option_index)) )
    {
        switch (c)
        {
            case 'h':
                usage(argv[0]) ;
                exit(0) ;
            case 0:     // If this option set a flag, do nothing else now.
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 2:
                host  = optarg;
                break;
            case 3:
                port  = atoi(optarg) ;
                break;
            case 15:
                nbThr = atoi(optarg) ;
                break;
            case 16:
                nbClt = atoi(optarg) ;
                break;
            case 17:
                tSpan = atoi(optarg) ;
                break;
            case 20:
                users = atoi(optarg) ;
                break;
            case 21:
                res = atoi(optarg) ;
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
            printf("Use -users parameter to specify the number of users\n");
            printf("and -port parameter to specify the SMTP port number\n");
            printf("and -host parameter to specify the SMTP host\n");
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


