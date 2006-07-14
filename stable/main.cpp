#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include "distribute/MailStore.h"
#include "loadgen/LoadGen.h"


int usage( const char* prog ) ;


typedef struct config_s {
    int     init_only ;
    char*   smtp_server ;
    int     smtp_port ;
    char*   pop3_server ;
    int     pop3_port ;
    char*   sink_server ;
    int     sink_port ;
    char*   local_domain ;
    char*   remote_domain ;
    char*   user_prefix ;
    char*   user_passwd ;
    int     user_start ;
    int     user_end ;
    int     init_rest ;
    int     threads ;
    int     clients ;
    int     span ;
    int     warmup ;
    int     rampdown ;
    int     users ;
    int     resolution ;
    int     is_filled ;
    config_s() {    // default values
       init_only    = 0;
       smtp_server  = "localhost" ;
       smtp_port    = 25 ;
       pop3_server  = "localhost" ;
       pop3_port    = 110 ;
       sink_server  = "localhost" ;
       sink_port    = 25 ;
       local_domain = "localdomain" ;
       remote_domain= "remotedomain" ;
       user_prefix  = "user" ;
       user_passwd  = "user" ;
       user_start   = 1 ;
       user_end     = 100 ;
       init_rest    = 12 ;
       threads      = 1 ;
       clients      = 1 ;
       span         = 1 ;
       warmup       = 1 ;
       rampdown     = 1 ;
       users        = 100 ;
       resolution   = 1 ;
       is_filled    = 0 ;
    } ;
} config_t ;

    int
main (int argc, char **argv)
{
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }

    config_t cfg ;
    int     iflag=0, smtp_port=0, users=0, idx=0, c=0, nbThr=1, nbClt=1,res=1 ;
    int     tSpan=0 ;
    char*   smtp_server=0 ;
    bool    is_filled=false ;
    int     option_index = 0;

    struct option long_options[] =
    {
        {"initonly"     , 0, &cfg.init_only, 1},
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
                cfg.smtp_server = optarg;
                break;
            case 3:
                cfg.smtp_port   = atoi(optarg) ;
                break;
            case 15:
                cfg.threads     = atoi(optarg) ;
                break;
            case 16:
                cfg.clients     = atoi(optarg) ;
                break;
            case 17:
                cfg.span        = atoi(optarg) ;
                break;
            case 20:
                cfg.users       = atoi(optarg) ;
                break;
            case 21:
                cfg.resolution  = atoi(optarg) ;
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
    // Pre-populez `users` mailBoxuri pe smtp_server:smtp_port folosind `nbThr` threaduri
    if( cfg.init_only )
    {
        if( !cfg.users || !cfg.smtp_port ){
            printf("Use -users parameter to specify the number of users\n");
            printf("and -smtpserver parameter to specify the SMTP host\n");
            printf("and -smtpport parameter to specify the SMTP port number\n");
            exit(2);
        }
        MailStore ms("./data/md.csv") ;
        ms.init( cfg.smtp_server, cfg.smtp_port, cfg.users, cfg.threads ) ;
        cfg.is_filled = true ;
        exit(0);
    }
    LoadGen::Smtp smtpGen ;
    smtpGen.init( cfg.clients, cfg.threads, cfg.resolution, tSpan ) ;
    smtpGen.run( ) ;
/*
    bench.cron.callback(tick) ;
    bench.run( 80,  is_filled ) ;
    bench.run( 100, is_filled ) ;
    bench.run( 120, is_filled ) ;
*/
    return 0;
}

int
usage(const char* app)
{
    printf("Usage %s [OPTIONS]\n", app );
    printf("\t-h\t\t\tHelp message\n" ) ;
    printf("\t-initonly\t\tInit storage\n" ) ;
    printf("\t-threads threads\tNumber of threads used for[gotta think of]\n" );
    printf("\t-clients clients\tNumber of clients\n" ) ;
    printf("\t-resolution sec\t\tResolution time\n" ) ;
    printf("\t-span sec\t\ttest Span in seconds\n" ) ;
    printf("\t-users users\t\tNumber of users on server\n" );
    printf("\t-smtpserver\t\tThe host name of the SMTP server\n");
    printf("\t-smtpport\t\tThe host port number of the SMTP server\n");
    printf("\t-pop3server\t\thost name of the POP3 server\n");
    printf("\t-pop3port\t\tport number of the POP3 server\n");
    printf("\t-sinkserver\t\thost name of the SINK server\n");
    printf("\t-sinkport\t\tport number of the SINK server\n");
    printf("\t-localdomain\t\tdomain name that represents local mail on the mail server\n");
    printf("\t-remotedomain\t\tdomain name that represents remote mail on the mail server\n");
    printf("\t-userprefix\t\tusername has the form USERPREFIX_userid\n") ;
    printf("\t-userpasswd\t\tusername password\n") ;
    printf("\t-userstart\t\tstarting number of userid\n") ;
    printf("\t-userend\t\tending number of userid\n") ;
    printf("\t-initrest\t\tminutes to wait after completing mailbox initialization\n") ;

    return 0 ;
}
