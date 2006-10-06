#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

void dostuff(int, FILE*); /* function prototype */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;
    int opt=0,val=0;
    FILE* f = fopen("concount.txt", "w" ) ;
    if(!f){ printf("cant open concount.txt\n"); exit(2); }
    setvbuf( f, (char *)NULL, _IONBF, 0 ) ;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if( val == -1 ) error("setsockopt reuseaddr");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1) {
        newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            dostuff(newsockfd, f);
            fclose(f);
            exit(0);
        }
        else close(newsockfd);
    } /* end of while */
    return 0; /* we never get here */
}

void dostuff (int sock, FILE* f)
{
    int n;
    char buffer[256];
    int ok =1 ;
    n= write( sock, "220 MailSink ready\r\n",20 ) ;
    if (n < 0) error("ERROR writing to socket");

    while( ok )
    {
        bzero(buffer,256);
        n = read(sock,buffer,255);
        printf("cmd: %s\n", buffer);
        if (n  < 0) { error("ERROR reading from socket"); ok=0; }
        if (n == 0) { printf("Server closed socket"); ok=0; }
        if( !strncasecmp(buffer, "EHLO", 4 )  ) {
            n= write( sock, "250-MailSink hello\r\n250-SIZE 10485760\r\n250 OK\r\n", 47 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        if( !strncasecmp(buffer, "MAIL FROM:", 10 )  ) {
            n= write( sock, "250 OK\r\n", 8 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        if( !strncasecmp(buffer, "RCPT TO:", 8 )  ) {
            n= write( sock, "250 OK\r\n", 8 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        if( !strncasecmp(buffer, "NOOP", 4 )  ) {
            n= write( sock, "250 OK\r\n", 8 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        if( !strncasecmp(buffer, "DATA", 4 )  ) {
            n= write( sock, "354 OK\r\n",8 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }

            do{
                bzero(buffer,256);
                n = read(sock,buffer,255);
                printf("cmd: %s\n", buffer);
                if (n < 0) { error("ERROR reading from socket"); ok=0; }
                if (n == 0) { printf("Server closed socket"); ok=0; }
                if(strstr( buffer, ".\r\n")==buffer) break;
                if(strstr( buffer, "\r\n.\r\n")) break;
            }while(1) ;
            printf("got all data, moving on\n");
            fprintf(f,"%i", 1) ;
            // Log that all went ok, and count at the end of test
            n= write( sock, "250 OK\r\n",8 ) ;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        if( !strncasecmp(buffer, "QUIT", 4 )  ) {
            n= write( sock, "221 Cya\r\n",9 ) ;
            ok=0;
            if (n < 0) { error("ERROR writing to socket"); ok=0; }
            continue;
        }
        n= write( sock, "500 Bad\r\n",9 ) ;
        if (n < 0) { error("ERROR writing to socket"); ok=0; }
    }
}
/*
   MAIL FROM:
   RCPT TO:
   NOOP
   BDAT 228 LAST
   QUIT
   */
