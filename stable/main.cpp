#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include "structs.h"
#include "distribute/MailStore.h"
#include "loadgen/LoadGen.h"
#include <netdb.h>
       #include <sys/types.h>
       #include <sys/wait.h>


int     usage( const char* prog ) ;
void*   runSmtp(void*);
void*   runPop3(void*);


    int
main (int argc, char **argv)
{
    if( argc < 2 ) { usage(argv[0]); exit(1) ; }

    config_t cfg ;
    int      idx=0, c=0 ;
    int      option_index = 0;

    struct option long_options[] =
    {
        {"initonly"     , 0, &cfg.init_only, 1},
        {"smtpserver"   , 1, 0, 2}, // DONE
        {"smtpport"     , 1, 0, 3}, // DONE
        {"pop3server"   , 1, 0, 4}, // DONE
        {"pop3port"     , 1, 0, 5}, // DONE
        {"sinkserver"   , 1, 0, 6}, // TODO
        {"sinkport"     , 1, 0, 7}, // TODO
        {"localdomain"  , 1, 0, 8}, // TODO
        {"remotedomain" , 1, 0, 9}, // TODO
        {"userprefix"   , 1, 0,10}, // TODO
        {"userpasswd"   , 1, 0,11}, // TODO
        {"userstart"    , 1, 0,12}, // ???
        {"userend"      , 1, 0,13}, // ???
        {"initrest"     , 1, 0,14}, // TODO
        {"threads"      , 1, 0,15}, // ??? is this needed ?
        {"clients"      , 1, 0,16}, // ???
        {"span"         , 1, 0,17}, // DONE
        {"warmup"       , 1, 0,18}, // TODO
        {"rampdown"     , 1, 0,19}, // TODO
        {"users"        , 1, 0,20}, // DONE
        {"resolution"   , 1, 0,21}, // TODO
        {"repeatchk"    , 1, 0,22}, // TODO

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
                if( long_options[option_index].flag != 0 ) break;
                printf ("option %s", long_options[option_index].name);
                if( optarg ) printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 2:
                cfg.smtp_server = optarg;
                break;
            case 3:
                cfg.smtp_port   = atoi(optarg) ;
                break;
            case 4:
                cfg.pop3_server = optarg;
                break;
            case 5:
                cfg.pop3_port   = atoi(optarg) ;
                break;
            case 14:
                cfg.init_rest   = atoi(optarg) ;
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
            case 22:
                cfg.repeat_chk= atoi(optarg) ;
                break;

            case ':':   // -H -P -U -T -C without operand
                fprintf( stderr, "Option %c requires a parameter.\n", optarg ) ;
                usage(argv[0]) ;
                exit(2) ;
            case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt );
                usage(argv[0]) ;
                exit(2) ;
            default:
                break ;
        }
    }

    for( idx=optind; idx < argc; idx++)
        printf ("Non-argument %s\n", argv[idx]);

    // Pre-populez `users` mailBoxuri pe smtp_server:smtp_port folosind `nbThr` threaduri
    // TODO: make this a function
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
        if(cfg.init_rest)
        {
            printf("Wait %i minutes to let mails reach the storage.", cfg.init_rest ) ;
            sleep(cfg.init_rest);
        }
        exit(0);
    }


    // smtpserver, into dest pointer
    sockaddr_in     dest ;  // man 7 ip
    struct hostent* host ;

    dest.sin_port   = htons( cfg.smtp_port ) ;
    dest.sin_family = AF_INET ;

    host = gethostbyname( cfg.smtp_server ) ;//not thread safe?
    if( !host ) return 2 ;

    memcpy( (char*)&dest.sin_addr, (char*)host->h_addr, host->h_length ) ;
    cfg.dest = &dest ;


    // pop3server, into dtest pointer
    sockaddr_in     pdest ;  // man 7 ip

    pdest.sin_port   = htons( cfg.pop3_port ) ;
    pdest.sin_family = AF_INET ;

    host = gethostbyname( cfg.pop3_server ) ;//not thread safe?
    if( !host ) return 2 ;

    memcpy( (char*)&pdest.sin_addr, (char*)host->h_addr, host->h_length ) ;
    cfg.pdest = &pdest ;


    pid_t spid, ppid;
    spid = fork();
    //ppid = fork();
    int rc = 0 ;
    int status ;
    

    if (spid >= 0 ) /* fork succeeded */
    {
        if (spid == 0  ) /* fork() returns 0 to the child process */
        {
            printf("CHILD: I am the child process!\n");
            printf("CHILD: Here's my PID: %d\n", getpid());
            printf("CHILD: My parent's PID is: %d\n", getppid());
            printf("CHILD: The value of my copy of spid is: %d\n", spid);
            printf("CHILD: Sleeping for 1 second...\n");
            ppid = fork() ;
            if( ppid >= 0 )
            {
                if(ppid==0)runPop3(&cfg);
            }
            runSmtp(&cfg);
            printf("CHILD: Enter an exit value (0 to 255): ");
            scanf(" %d", &rc);
            printf("CHILD: Goodbye!\n");    
            exit(rc); /* child exits with user-provided return code */
        }
        else /* fork() returns new pid to the parent process */
        {
            printf("PARENT: I am the parent process!\n");
            printf("PARENT: Here's my PID: %d\n", getpid());
            printf("PARENT: The value of my copy of spid is %d\n", spid);
            printf("PARENT: I will now wait for my child to exit.\n");
            wait(&status); /* wait for child to exit, and store its status */
            //waitpid(-1, &status, 0);
            printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
            printf("PARENT: Goodbye!\n");             
            exit(0);  /* parent exits */       
        }
    }
    else /* fork returns -1 on failure */
    {
        perror("fork"); /* display error message */
        exit(0); 
    }
    
    // First try to start LoadGen::Smtp
#if 0
    pthread_t smtp_thread, pop3_thread ;

    rc = pthread_create( &smtp_thread, 0, runSmtp, (void*)&cfg ) ;
    if( rc == 0 ) pthread_detach( smtp_thread) ;
    else { printf( "ERROR creating smtp_thread\n" ) ;  exit(2) ; }

    // Then start LoadGen::Pop3
    rc = pthread_create( &pop3_thread, 0, runPop3, (void*)&cfg ) ;
    if( rc == 0 ) pthread_detach( pop3_thread) ;
    else { printf( "ERROR creating pop3_thread\n" ) ; exit(2) ; }
#endif
    sleep(10);
    return 0;
}

///

    void*
runSmtp( void* a )
{
    printf("starting smtp_gen\n" ) ;
    config_t* cfg = (config_t*) a ;
    LoadGen::Smtp smtpGen ;
    smtpGen.init( cfg ) ;
    smtpGen.run( ) ;
}

    void*
runPop3( void* a )
{
    printf("starting pop3_gen\n" ) ;
    config_t* cfg = (config_t*) a ;
    LoadGen::Pop3 pop3Gen ;
    pop3Gen.init( cfg ) ;
    pop3Gen.run( ) ;
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
    printf("\t-smtpserver host\t\tThe host name of the SMTP server\n");
    printf("\t-initrest mins\t\tWait [mins] minutes after a populating the storage\n");
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
    printf("\t-repeatchk\t\tInterval in seconds for repeated pop3 checks\n") ;
    printf("\t-initrest\t\tminutes to wait after completing mailbox initialization\n") ;

    return 0 ;
}
