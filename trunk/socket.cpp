#include "socket.h"

Socket::Socket()
{
      RING;
      socket_i =-1;
      timeout  = 1;
}

Socket::~Socket()
{
      RING;
}


void
Socket::create( const int family_i, const int type_i, const int protocol_i)
{
      RING;
      int retval =socket( family_i, type_i, protocol_i);
      if( retval == -1)
      {
            stringstream exception;
            exception <<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<":"<<strerror( errno);
            throw ( char*)exception.str().c_str();
      }
      socket_i =retval;
}



void
Socket::connect( const string& hostName_cs, uint16_t port_ui)
{
      RING;
      if( socket_i ==-1)
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
            stringstream exception;
            exception <<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<":"<<strerror( errno);
            throw (char*)exception.str().c_str();
      }

      ( void) memcpy( ( char*)&whereTo.sin_addr, ( char*)host->h_addr, host->h_length);
      int retval =::connect( socket_i, ( sockaddr*)&whereTo, sizeof(whereTo));
      if( retval != 0)
      {
            stringstream exception;
            exception <<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<":"<<strerror( errno);
            throw ( char*)exception.str().c_str();
      }
}



void
Socket::send( const string& message)
{
      RING;
      if( socket_i ==-1)
      {
            throw "Socket not created.";
      }

      if( message.empty())
      {
            throw "Can't send empty string";
      }

      int retval =::send( socket_i, message.c_str(), message.size(), MSG_NOSIGNAL);
      if( retval == -1)
      {
            throw strerror( errno);
      }
}




void
Socket::close( void)
{
      RING;
      int retval =shutdown( socket_i, 2);
      if( retval == -1)
      {
            stringstream exception;
            exception <<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<":"<<strerror( errno);
            throw ( char*)exception.str().c_str();
      }
}



void
Socket::recv( string& response)
{
      RING;
      char recvBuffer_cp[ 1024+1];
      timeval sleepTime;
      fd_set readable, er;
      response ="";
      for(;;)
      {
            bzero( recvBuffer_cp, 1024+1);
            FD_ZERO( &readable);
            FD_ZERO( &er);
            FD_SET ( socket_i, &readable);
            FD_SET ( socket_i, &er);

            sleepTime.tv_sec  = 1; // n number of seconds to sleep.
            sleepTime.tv_usec = 0; // n number of micro seconds to sleep.

            int retVal =select( socket_i +1, &readable, NULL, &er, &sleepTime);

            if( retVal == -1)
            {
                  throw strerror( h_errno);
            }

            if( retVal != 0)
            {
                  retVal =read( socket_i, recvBuffer_cp, 1024);
                  response +=recvBuffer_cp;
                  return;
            }
            else
            {
//                  stringstream exception;
//                  exception <<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<":"<<"select() timed out";
                  cout <<"RECV select() timed out" <<endl;
                  return;
            }

      }
}



/*
Socket::setOptions();

Socket::getOptions();
*/

