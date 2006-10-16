/**
 *   \brief    Remote shell.
 *   \see      rsh.c
 *   \author   Cristina Balan, Andrei Paduraru, Marius Negreanu
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "util.h"
#include "socket.h"
#include "rshdaemon.h"

int sockfd, newsockfd;
char start[MAX_LIN];
char stop[MAX_LIN];
char restart[MAX_LIN];
char axi_fresh[MAX_LIN];

typedef struct {
    char* keyword;
    int  (*call)(int socket, char* buf);
} Callbacks ;


Callbacks callbacks[] = {
    {"COPY", callback_copy } ,
    {"RM", callback_rm } ,
    {"MKDIR", callback_mkdir},
    {"EXECUTE",callback_execute},
    {"CHECKCORE", callback_checkCore},
    {"START", callback_start},      //! @todo
    {"STOP", callback_stop},        //! @todo
    {"REFRESH",callback_refresh},   //! @todo
    {NULL, NULL}
} ;


/**
 * CALLBACK Functions, to deal with client requests. 
 */

int
callback_start( int sock, char* buf)
{
    int timeout=0, rb;
    char* startCmd=NULL;

    rb = sscanf(buf, "%d", &timeout) ;
    startCmd = strchr(buf,' ');
    if( rb != 1 || !startCmd ) {printf("Invalid syntax for START\n"); return FALSE;}

    util_axiStart(TEST_LOCAL, timeout, startCmd);
    sock_sendStatus(sock,0);
    return TRUE ; //! @todo pls return a proper value(sendStatus)
}


int
callback_stop( int sock, char* buf)
{
    int timeout=0, rb;
    char* stopCmd=NULL;

    rb = sscanf(buf, "%d", &timeout) ;
    stopCmd = strchr(buf,' ');
    if( rb != 1 || !stopCmd ) {printf("Invalid syntax for STOP\n"); return FALSE;}

    util_axiStop(TEST_LOCAL, timeout, stopCmd);
    sock_sendStatus(sock,0);
    return TRUE ;
}

/**
 * \todo Replace this with a newer version 
 */
int
callback_refresh(int sock, char *buf )
{
    char com[MAX_LIN] = {0};
    struct stat stats ;
    char cale_s[MAX_LIN]={0}, cale_d[MAX_LIN]={0} ;
    int cod=0, rb=0;

    rb = sscanf(buf, "%s %s", cale_s, cale_d) ;
    cod = stat(cale_s, &stats);
    if(cod!=0) {printf("rsh: Error: Can't find %s\n", cale_s ) ; exit(-1) ;}
    sprintf( com, "rm -rf %s", cale_d);
    if (system(com) == -1) {
        return errno;
    }
    sprintf( com, "/bin/cp -r %s %s", cale_s, cale_d);
    if (system(com) == -1) {
        return errno;
    }
    sock_sendStatus(sock,0);
    return 0;
}



/**
 * \brief Read a stream for socket, and writes it to dest_dir/src_file.
 */
int
callback_copy(int sock, char* buf)
{
    int f, bw,len, rb;             // bytes written to disk
    char buff[BSIZE]={0};
    char dest_file[PATH_MAX]={0};
    char *src_file, *dest_dir ; 

    rb = sscanf(buf, "%s %s %d", src_file, dest_dir, &len) ;
    if( rb != 3 || len<=0) {printf("Invalid syntax for COPY\n"); return FALSE;}

    sprintf(dest_file,"%s/%s", dest_dir, src_file );
    f = creat(dest_file, S_IRUSR | S_IWUSR);
    if (f < 0) {
        sock_sendStatus(sock, errno);
        return errno;
    }
    while (len>0 && (bw = read(sock, buff, BSIZE))) {
        int ret = write(f, buff, bw<len?bw:len);
        if (bw < 0) {
            perror("Transmission error");
            close(f);
            return errno;
        }
        if (ret < 0) {
            close(f);
            return errno;
        }
        len -= bw ;
    }
    sock_sendStatus(sock, 0);
    close(f);
    return 0;
}



int
callback_rm(int sock, char *buf)
{
    char cmd[MAX_LIN]={0};
    int rb ;
    char path[MAX_LIN]={0} ;

    rb = sscanf(buf, "%s", path) ;
    if( rb != 1) {printf("Invalid syntax for RM\n"); return FALSE; }

    sprintf(cmd, "rm -rf %s", path );
    sock_sendStatus(sock, 0);// fix this pls
    return system(cmd);
}



int
callback_mkdir(int sock, char *buf)
{
    int rb;
    char path[MAX_LIN]={0};

    printf("MKDIR '%s'\n", path);
    rb = sscanf(buf, "%s", path) ;
    if( rb != 1) {printf("Invalid syntax for MKDIR\n"); return FALSE ; }

    if (mkdir(path, 0777) == -1) {
        printf("error:[%s]", strerror(errno));
        sock_sendStatus(sock, errno);
        return errno;
    }
    sock_sendStatus(sock, 0);
    return 0;
}

/**
 * @todo Look at strchr
 */
int
callback_execute( int sock, char* buf )
{
    char* p =NULL;
    int rc=0;

    p=buf;
    if(!p) {printf("null p\n"); return -1; }
    rc= system(p);
    sock_sendStatus(sock,0);
    return 0;
}



int
callback_checkCore( int sock, char* buf )
{
    char core_srcDir[PATH_MAX]={0}, dbg_srcDir[PATH_MAX]={0},
         axi_workDir[PATH_MAX]={0}, axi_cfgFile[PATH_MAX]={0}, crash_destDir[PATH_MAX]={0} ;
    int rc = FALSE ;
    sscanf( buf, "%s %s %s %s %s", core_srcDir, dbg_srcDir, axi_workDir, axi_cfgFile, crash_destDir );
    rc = util_checkCoreLocal( core_srcDir, dbg_srcDir, axi_workDir, axi_cfgFile, crash_destDir );
    sock_sendStatus(sock, rc);//! @todo figure out what status i should send
    return FALSE;// means no core was found
}



void
callback_client(int socket )
{
    char buf[8192] ={0};
    int ok = 1;
    int n,i;


    while( ok ) {
        char* p=NULL ;
        memset(buf,0, 8192);
        n = read(socket, buf,8191) ;

        if (n  < 0) { fprintf( stderr, "ERROR reading from socket\n"); ok=0; }
        if (n == 0) { /*printf("Client closed socket\n"); */ok=0; break;}
        buf[n] = '\0' ;

        //extract keyword
        p = strchr( buf, ' ');
        if(p){
            *p = '\0' ;
            p++;
            for(i=0; *(p+i) >= 32 && *(p+i)!= '\0'; ++i)
            ;
            *(p+i)='\0';
            printf("cmd[%s] param[%s]\n", buf, p);
        }else {
            printf("\n--Unknown command--\n");
            continue;
        }
        for(i=0;callbacks[i].keyword != NULL; i++ )
            if(util_matches(buf, callbacks[i].keyword) ) {
                callbacks[i].call(socket, p); break;
            }
    }
}


int
callback_rsh(int portno)
{
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int cod;


    printf("Running on port:%d\n", portno);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "rsh: Error: opening socket: %s\n",
                strerror(errno));
        return 1;
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "rsh: Error on binding: %s\n", strerror(errno));
        exit(-1);
    }
    cod = listen(sockfd, 5);
    if (cod < 0)
    {
        printf("rsh: Error:%s\n", strerror(errno));
        exit(-1);
    }
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            fprintf(stderr, "rsh: Error on accept: %s",
                    strerror(errno));
            exit(-1);
        }
        callback_client(newsockfd);
    }
}



void
intercept()
{
    printf("Receive broken pipe!!!\n");
}



int
rshDaemon( int port )
{

    pid_t pid, sid;
    if(port <=0 || port >65535 ) { fprintf(stderr, "Cant bind port: %d\n", port);return -1;}
    /* Daemon */
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "rsh: Error while forking\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {                           /* Parent */
        exit(EXIT_SUCCESS);
    }
    /* Child code */
    umask(0);
    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);
    if ((chdir("/")) < 0)
    {
        fprintf(stderr, "rsh: Can't change to /\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, util_terminare) == SIG_ERR)
    {
        perror("signal() error!");
        return 1;
    }

    if (signal(SIGPIPE, intercept) == SIG_ERR)
    {
        perror("signal() error!");
        return 1;
    }
    printf("Closing fd's\n");
#if 0
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    stdout = fopen("/dev/null", "w");
    stderr = fopen("/dev/null", "w");
#endif
    /* Daemon */
    callback_rsh(port);
    return 0;
}


