/**
 *   \brief    Wraps around rm command.
 *   \see      putgenhost.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 **/

#include "util.h"
#include "socket.h"

struct globals_s glob ;
static int rm_parseArgs( int argc, char* argv[] ) ;

int main(int argc, char *argv[])
{
    char *tc;                   // conexion type
    char comand[MAX_LIN], *host;
    int port;

    if (argc < 2)              // trebuie sa am sursa si destinatie
    {
        printf("rm: missing file operand\n");
        printf("Try `rm -h` for more information.\n");
        return 1;
    }
    rm_parseArgs( argc, argv);
    util_isEnv(PUT_TTYPE);
    tc = getenv(PUT_TTYPE);
    if (argc < 2)
    {
        fprintf(stderr, "Error: no valid sintax");
        return 1;
    }

    if (!strcmp(tc, "local"))
    {
        strcpy(comand, "/bin/rm -rf ");
        strcat(comand, argv[1]);
        return system(comand);
    } else if (!strcmp(tc, "remote"))
    {
        util_isEnv(PUT_HOST);
        util_isEnv(PUT_PORT);
        host = getenv(PUT_HOST);
        port = atoi(getenv(PUT_PORT));
        if (!strcmp(argv[1], "-R") || !strcmp(argv[1], "-r"))
        {
            return client_rm(host, port, argv[2]);
        } else
        {
            return client_rm(host, port, argv[1]);
        }

    } else
        printf("Invalid $put_ttype\n");
    return 1;
}



void rm_usage(void) {

    printf("Usage: rm [OPTION] FILE...\n");
    printf("Remove (unlink) the FILE(s).\n");
    printf("\n");
    printf("  -v, --verbose     print a message for each action executed\n");
    printf("  -h, --help        display this help and exit\n");
    exit(0);
}



static int
rm_parseArgs( int argc, char* argv[] )
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
            setenv("put_ttype", optarg, 1);
            break;
        case 'H':
            //glob.hostname = optarg;
            setenv("put_host", optarg, 1);
            break;
        case 'P':
            //glob.port = atoi(optarg);   // fixme
            setenv("put_port", optarg, 1);
            break;
        case 'h':
            rm_usage();
        case 'v':
            glob.verbose = TRUE;
            break;
        }
    }
    return TRUE;
}
