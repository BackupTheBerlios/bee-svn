/**
 *   \brief    Handles the refreshing of ptgen state(users/domains).
 *   \see      ptgenhost.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "util.h"
#include "socket.h"


struct globals_s glob;
static int rc_parseArgs( int argc, char* argv[] ) ;



int client_refresh(char *host, int port, char *sursa, char *dest)
{
    char command[MAX_LIN] = {0};
    int cod, sockfd;

    sockfd = sock_connectTo( host, port ) ;

    /*REFRESH*/
    sprintf(command, "REFRESH %s %s", sursa, dest);
    sock_sendLine( sockfd, command );
    cod = sock_getStatus( sockfd );
    if (cod < 0)
    {
        fprintf(stderr, "Error: %s\n", strerror(cod));
        close(sockfd);
        return cod;
    }

    close(sockfd);
    return 0;
}



static int pt_refresh(int test_type, char *source, char *dest, char *host, int port)   // Cristina
{
    char cmd[MAX_LIN] = { 0 };
    struct stat buf;
    int cod = 0;

    cod = stat(source, &buf);
    if (cod != 0)
    {
        fprintf(stderr, "* refresh_client: Error: %s: %s\n", source, strerror(errno));
        exit(-1);
    }
    if (test_type == TEST_LOCAL)
    {
        printf("* refresh_client: Removing %s\n", dest);
        sprintf(cmd, "/bin/rm -rf %s", dest);
        if (system(cmd))
        {
            printf("* refresh_client: Error: %s not deleted\n", dest);
            return 1;
        }
        printf("* refresh_client: copying %s to %s\n", source, dest);
        if(dest[strlen(dest)-1] =='/' ) dest[strlen(dest)-1] = '\0';
        sprintf(cmd, "/bin/cp -R %s %s", source, dest);
        if (system(cmd))
        {
            printf("* refresh_client: Error: File not copied\n");
            return 1;
        }
        return 0;
    }

    if (test_type == TEST_REMOTE)
    {
        if (client_refresh(host, port, source, dest))
        {
            printf("* refresh_client: Error: Could not make refresh!!!\n");
            return EXIT_FAILURE;
        }
        return 0;
    }
    return 0 ;
}


int main(int argc, char* argv[])
{
    char *tc;
    char *path, *host;
    int port;

    rc_parseArgs( argc, argv);
    util_isEnv(PUT_TTYPE);
    util_isEnv(PUT_STOP);
    util_isEnv(PUT_START);
    util_isEnv(PUT_DEFDOM);
    util_isEnv(PUT_WORKDIR);
    tc = getenv(PUT_TTYPE);

    if (!strcasecmp(tc, "local"))
    {
        printf("* refresh_client: Working local\n");
        util_ptStop(TEST_LOCAL, 5, getenv(PUT_STOP)); //! @todo replace 5 with a proper timeout
        pt_refresh(TEST_LOCAL, getenv(PUT_DEFDOM), getenv(PUT_WORKDIR), 0, 0);
        util_ptStart(TEST_LOCAL, 5, getenv(PUT_START));//! @todo replace 5
    } else if (!strcasecmp(tc, "remote"))
    {
        printf("* refresh_client: Working remote\n");
        util_isEnv(PUT_HOST);
        util_isEnv(PUT_PORT);

        host = getenv(PUT_HOST);
        port = atoi(getenv(PUT_PORT));
        path = getenv(PUT_WORKDIR);
        util_ptStop(TEST_REMOTE, 5, getenv(PUT_STOP)); //! @todo replace 5
        pt_refresh(TEST_REMOTE, getenv(PUT_DEFDOM), getenv(PUT_WORKDIR), host,
                    port);
        util_ptStart(TEST_REMOTE, 5, getenv(PUT_START));//! @todo replace 5
    } else
    {
        printf("* refresh_client : Invalid $pt_ttype\n");
        return 1;
    }
    return EXIT_SUCCESS;
}
void rc_usage(void) {

    printf("Usage: refresh_client [OPTION] COMMAND...\n");
    printf("Refresh the state of Axigen.\n");
    printf("\n");
    printf("  -v, --verbose     print a message for each action executed\n");
    printf("  -h, --help        display this help and exit\n");
    exit(0);
}



static int
rc_parseArgs( int argc, char* argv[] )
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
            setenv("pt_ttype", optarg, 1);
            break;
        case 'H':
            //glob.hostname = optarg;
            setenv("pt_host", optarg, 1);
            break;
        case 'P':
            //glob.port = atoi(optarg);   // fixme
            setenv("pt_port", optarg, 1);
            break;
        case 'h':
            rc_usage();
        case 'v':
            glob.verbose = TRUE;
            break;
        }
    }
    return TRUE;
}

