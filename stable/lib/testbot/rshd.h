#ifndef RSHDAEMON_H
#define RSHDAEMON_H

int rsh_main( int port );
static int callback_socket(int portno)  ;
static int callback_command(int socket );

#endif
