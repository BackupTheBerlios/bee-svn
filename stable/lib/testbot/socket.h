#include "config.h"
int sock_connectTo( char* host, int port ) ;
int sock_sendStatus(int sock, int cod);
int sock_getStatus( int sock ) ;
int sock_sendLine(int sock, char *line);
