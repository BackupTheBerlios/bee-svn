/**
 *   \brief     Wraps around copy operations.
 *   \see       
 *   \author    Cristina Balan, Andrei Paduraru
 *   \date      Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 **/
#include "util.h"
#include "socket.h"
#include <libgen.h>

struct globals_s glob ;
static int cp_parseArgs( int argc, char* argv[] );

static int copy(int sock, char *src_file)
{
    int f = open(src_file, O_RDONLY);
    if (f < 0)
    {
        fprintf(stderr, "cp: Error: Cannot open file %s : %s\n", src_file,
                strerror(errno));
        return errno;
    }
    char buff[DIM_BUFF];
    int t, w, q;

    while ((w = read(f, buff, DIM_BUFF)) > 0)
    {
        t = 0;
        while (t < w)
        {
            if ((q = write(sock, buff + t, w - t)) < 0)
            {
                fprintf(stderr, "cp: Error: Could not sent the file %s: %s\n",
                        src_file, strerror(errno));
            }
            t += q;
        }
    }
    close(f);
    return 0;
}


/*
 * Send a file to socket.
 */
static int client_copy(char *host, int port, char *src_file, char *dest_dir)
{
    int cod;
    char cmd[MAX_LIN]={0};
    char* bname, sockfd;

    sockfd = sock_connectTo(host,port); 

    int l = util_fileSize(src_file);
    if (l == -1) {
        fprintf(stderr, "cp: Error: '%s' in not a regular file\n", src_file);
        exit(-1);
    }
    bname = (char*)basename(src_file) ;
    sprintf(cmd, "COPY %s %s %d", bname, dest_dir, l ) ;
    sock_sendLine( sockfd, cmd );
    cod = copy(sockfd, src_file);
    if (cod)
    {
        close(sockfd);
        return 1;
    }

    cod = sock_getStatus( sockfd );

    if (cod<0) {
        fprintf( stderr, "cp: Error: Receiving command confirmation!\n");
        exit(-1);
        return 1;
    }
    return 0;
}



// o sa fie cp sursa destinatie
int main(int argc, char *argv[])
{
    // char *type;
    char *type;
    char *str;

    if (argc < 2)
    {
        printf("cp: missing file operand\n");
        printf("Try `cp -h` for more information.\n");
        return 1;
    }
    cp_parseArgs(argc, argv);
    util_isEnv(PUT_TTYPE);
    type = getenv(PUT_TTYPE);

    if (!strcmp(type, "local"))
    {
        str = (char *)malloc(strlen(argv[1]) + strlen(argv[2]) + 5);
        sprintf( str, "/bin/cp -R %s %s", argv[1], argv[2] ) ;
        return system(str);
    } else if (!strcmp(type, "remote"))
    {
        char *host;
        int port;

        util_isEnv(PUT_HOST);
        util_isEnv(PUT_PORT);
        host = getenv(PUT_HOST);
        port = atoi(getenv(PUT_PORT));
        return client_copy(host, port, argv[1], argv[2]);
    } else
        printf("cp: Error: Invalid $pt_ttype\n");
    return 1;
}

void cp_usage(void) {

    printf("Usage: cp [OPTION] COMMAND...\n");
    printf("Copy SOURCE to DEST\n");
    printf("\n");
    printf("  -v, --verbose     print a message for each action executed\n");
    printf("  -h, --help        display this help and exit\n");
    exit(0);
}



static int
cp_parseArgs( int argc, char* argv[] )
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
            cp_usage();
        case 'v':
            glob.verbose = TRUE;
            break;
        }
    }
    return TRUE;
}

