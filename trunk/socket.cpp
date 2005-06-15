#include "socket.h"
#include <errno.h>

Socket::Socket()
{
      // In case I call this.send without first calling create.
      socket_mi =-1;
}
Socket::~Socket(){}


void
Socket::create( const int family_i, const int type_i, const int protocol_i)
{
      int retval =socket( family_i, type_i, protocol_i);
      if( retval == -1)
      {
            //man 3 strerror #include <string.h>
            throw strerror( errno);
      }
      socket_mi =retval;
}


void
Socket::connect( const string& hostName_cs, uint16_t port_ui)
{
      if( socket_mi ==-1)
      {
            throw "Socket not created.";
      }
      sockaddr_in     whereTo;// man 7 ip
      struct hostent* host;

      whereTo.sin_family =AF_INET;
      whereTo.sin_port   =htons( port_ui);

      host =gethostbyname( hostName_cs.c_str());
      if( host ==( struct hostent*)NULL)
      {
           //exception ="Cannot resolve \"%s\" (%s)", hostName_cs, strerror( h_errno);
           //throw exception;
      }

      ( void) memcpy( ( char*)&whereTo, ( char*)host->h_addr, host->h_length);
      int retval =::connect( socket_mi, ( sockaddr*)&whereTo, sizeof(whereTo));
      if( retval != 0)
      {
            throw strerror( errno);
      }
}



void
Socket::send( const string& message)
{
      if( socket_mi ==-1)
      {
            throw "Socket not created.";
      }

      if( message.empty())
      {
            throw "Can't send empty string";
      }

      int retval =::send( socket_mi, message.c_str(), message.size(), MSG_NOSIGNAL);
      if( retval == -1)
      {
            throw strerror( errno);
      }
}




//              |^^Why void?
void
Socket::close( void)
{
      int retval =shutdown( socket_mi, 2);
      if( retval == -1)
      {
            throw strerror( errno);
      }
}

/*
Socket::setOptions();

Socket::getOptions();

Socket::recv( string& response)
{
      
}

*/

