/**
 *   \brief    Socket operations.
 *   \see      util.c 
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#include "socket.h"


int sock_connectTo( char* host, int port)
{
    struct sockaddr_in serv_addr;       // structure used for connection
    int sockfd;                 // socket descriptor
    struct hostent *server;


    // socket create
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error: can't create socket");   /* No \n ; see man perror */
        return -1;
    }
    // connected with server
    server = gethostbyname(host);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return -1;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr))
        < 0)
    {
        perror("Socket Error");
        return -1;
    }
    return sockfd ;
}


/**
 * send on socket Error: errno
 * or OK: Command successful*/
int sock_sendStatus(int sock, int cod)
{
    char str[MAX_LIN] = { 0 };
    if (!cod)
    {
        sprintf(str, "O:\r\n");
        write( sock, str, strlen(str) ) ;
        return 0 ;
    }
    sprintf(str, "E:%d\r\n", cod);
    return write(sock, str, strlen(str));
}

int sock_getStatus( int sock )
{
    char buf[MAX_LIN+1]={0};
    int status=0;
    read(sock, buf, MAX_LIN) ;
    if(buf[0]=='O') return 0;
    if(buf[0]=='E') sscanf( buf, "E:%d\r\n", &status );
    return status;
}

int sock_sendLine(int sock, char *line)
{
    int t;
    char tr[MAX_LIN]={0};

    sprintf(tr, "%s\r\n", line);
    t = write(sock, tr, strlen(tr));
    if (t < 0)
    {
        fprintf(stderr, "ERROR writing on socket : %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}



