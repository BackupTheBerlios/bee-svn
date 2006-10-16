#ifndef RSHDAEMON_H
#define RSHDAEMON_H

int callback_copy(int sock, char *buf) ;
int callback_mkdir(int sock, char *buf);
int callback_rm(int sock, char *buf);
int callback_refresh(int sock, char *buf) ;
int callback_start( int sock, char* buf ) ;
int callback_stop( int sock, char* buf ) ;
int callback_execute( int sock, char* buf ) ;
int callback_checkCore( int sock, char* buf ) ;
void callback_client(int socket );
int callback_rsh(int portno)  ;
int rshDaemon( int port );

#endif
