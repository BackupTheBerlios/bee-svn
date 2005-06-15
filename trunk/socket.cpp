#include "socket.h"
#include <errno.h>

Socket::Socket(){};
Socket::~Socket(){}

Socket::create( const int family_i, const int type_i, const int protocol_i)
{
      int retval =socket( family_i, type_i, protocol_i);
      if( retval == -1)
      {
            throw strerror( errno);//man 3 strerror #include <string.h>
      }
      socket_m =retval;
}


//                |^^^^^^^^^^a lot commod^^^^^^^^^^^^^^^|
Socket::connect( const string& hostName_cs, uint16_t port_ui) //for uint16#include <netinet/in.h>
{
      sockaddr_in     whereTo;//man 7 ip
      struct hostent* host;

      sockaddr.sin_family =AF_INET;
      sockaddr.sin_port   =htons( port_ui);

      host =gethostbyname(hostName_cs.c_str());
      if( host ==( struct hostent*)NULL)
      {
           exception ="Cannot resolve \"%s\" (%s)", hostName_cs, strerror( errno);
           throw exception;
      }

      (void)memcpy( ( char*)&whereTo, ( char*)host.h_addr, host.h_length);
      int retval =::connect( socket_m, ( sockaddr*)&whereTo, sizeof(addr));
      if( retval != 0)
      {
            throw strerror( errno);
      }
}


Socket::send( const string& message)
{
      //                  |^^ not checked if it created was called.
      int retval =::send( m_socket, message.str(), message.size(), MSG_NOSIGNAL);
      if( retval == -1)
      {
            throw strerror( errno);
      }
}




//              |^^Why void?
Socket::close( void)
{
      int retval =shutdown( socket_m, 2);
      if( retval == -1)
      {
            throw strerror( errno);
      }
}

/*
Socket::setOptions();

Socket::getOptions();

Socket::recv( string& response){}

*/

