#ifndef SOCKET_H
#define SOCKET_H

/*
struct sockaddr {
   sa_family_t    sa_family;
   char           sa_data[14];
}
Fields

sa_family
    The address family.

sa_data
    The address value.
*/

//    all the members has to throw exceptions
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;


class Socket {
      public:
            int m_socket;

      public:

            //the socket family
            //the socket type
            //the socet protocol
            Socket( const int family_i, const int type_i, const int protocol_i);

            ~Socket();//maybe close?


            // create();//the contructor has the job to create the scoket

            // adress_t :wrapper for sockaddr. the length is computed inside
            connect( address_t adress);// for TCP connection. return an error
                                       // if socket is UDP

            //bool send(const string& message_s, unsigned int flags_ui);//send for TCP, sendTO for UDP

            //this could return a string. or a string might be passed by reference
            //study wich is faster
            //bool recv( unsigned int flags_ui);//recv for TCP, recvFROM for UDP

            //bool close();//closes the socket
            //bool setOptions();//set socket options
            //bool getOptions();//get socket options
            // Operations

};
#endif
