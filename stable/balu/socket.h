#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // for uint16_t
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <string>

#include "debug.h"

using namespace std;

class Socket {
      public:
            int socket_i;
            int timeout;
      public:

            Socket( );
           ~Socket( );



            void create( const int family_i, const int type_i, const int protocol_i);//the contructor has the job to create the scoket

            // adress_t :wrapper for sockaddr. the length is computed inside
            void connect( const string& hostName, uint16_t port);// for TCP connection. return an error
                                       // if socket is UDP

            void send(const string& message_s);//send for TCP, sendTO for UDP

            //this could return a string. or a string might be passed by reference
            //study wich is faster
            void recv( string& response);//recv for TCP, recvFROM for UDP

            void close( );//closes the socket
            //bool setOptions();//set socket options
            //bool getOptions();//get socket options
            // Operations

};
#endif
