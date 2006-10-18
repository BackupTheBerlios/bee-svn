/**
 *   \brief  execute a given parameter, either on local machine, or remote
 *   \see    socket.c
 *   \author Cristina Balan, Andrei Paduraru
 *   \date Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "util.h"
#include "socket.h"
#include <wait.h>


struct globals_s glob ;
static int aa_parseArgs( int argc, char* argv[] ) ;


static int cli_main(char *host, int port, char *c)
{
    char cmd[MAX_LIN]={0};
    int cod, sockfd;

    /*REMOTE*/
    sockfd = sock_connectTo(host,port) ;

    sprintf( cmd, "EXECUTE %s", c ) ;
    sock_sendLine( sockfd, cmd );
    cod = sock_getStatus( sockfd );
    if (cod<0) {
        fprintf(stderr, "* axigen_action: Error: No remote host confirmation!\n");
        return errno;
    }
    if (cod>0) {
        fprintf(stderr, "* axigen_action: Error: '%s'\n", strerror(cod));
        fprintf(stderr, "* axigen_action: Error: Can't execute : '%s'\n", c);
        close(sockfd);
        return 1;
    }
    close(sockfd);
    return 0;
}


int main(int argc, char *argv[])
{
    char *hostname = 0, *ttype =0;
    int port=0;
    int cod=-1;
    glob.verbose = 1 ;
    if (argc < 2)
    {
        printf("axigen_action: missing operand\n");
        printf("Try `axigen_action -h` for more information.\n");
        return EXIT_FAILURE;
    }
    aa_parseArgs( argc, argv);
    if (signal(SIGINT, util_terminare) == SIG_ERR)
    {
        perror("* axigen_action: Signal error!");
        return EXIT_FAILURE;
    }

    util_isEnv(AXI_TTYPE);
    ttype = getenv(AXI_TTYPE ) ;

    /*LOCAL*/
    if(!strcasecmp(ttype, "local" ) )
    {
        cod = system(argv[1] ) ;
        exit( WEXITSTATUS(cod));
    }else
    if(!strcasecmp(ttype,"remote") )
    {
        util_isEnv(AXI_HOST);
        util_isEnv(AXI_PORT);
        hostname = getenv(AXI_HOST);
        port = atoi(getenv(AXI_PORT));
        cod = cli_main(hostname, port, argv[1]);
    }else
        printf("* axigen_action: Invalid test type\n");

    return -2 ;
}

void aa_usage(void) {

    printf("Usage: axigen_action [OPTION] COMMAND...\n");
    printf("Send a COMMAND to the testbot daemon\n");
    printf("\n");
    printf("  -v, --verbose     print a message for each action executed\n");
    printf("  -h, --help        display this help and exit\n");
    exit(0);
}
static int
aa_parseArgs( int argc, char* argv[] )
{
    int c;
    while ((c = getopt(argc, argv, "t:H:P:hv")) != -1)
    {
        switch (c)
        {
        case 't':
            if (!strcasecmp(optarg, "remote"))
                //glob.test_type = TEST_REMOTE;
            if (!strcasecmp(optarg, "local"))
                //glob.test_type = TEST_LOCAL;
            /*if (!glob.test_type) {
                printf("* testbot: Error: Give valid context local/remote.\n");
                tb_usage();
            }*/
            setenv("axi_ttype", optarg, 1);
            break;
        case 'H':
            //glob.hostname = optarg;
            setenv("axi_host", optarg, 1);
            break;
        case 'P':
            //glob.port = atoi(optarg);   // fixme
            setenv("axi_port", optarg, 1);
            break;
        case 'h':
            aa_usage();
        case 'v':
            glob.verbose = TRUE;
            break;
        }
    }
    return TRUE;
}
