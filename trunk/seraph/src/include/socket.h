#if !defined SOCKET_H
#define SOCKET_H

int sock_connectTo( const char* host, const int port ) ;
int sock_sendStatus(const int sock, const int cod);
int sock_getStatus( const int sock ) ;
int sock_sendLine(const int sock, const char *line);

#else
        #warning "*** Header allready included ***"
#endif
