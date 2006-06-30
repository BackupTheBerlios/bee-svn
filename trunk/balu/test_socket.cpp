#include "socket.h"
#include <iostream>
using namespace std;
int main()
{
      try
      {
            Socket sock_m;
            sock_m.create( AF_INET, SOCK_STREAM, 0);
            sock_m.connect( "localhost", 1430);
            string resp="";
            sock_m.recv( resp);
            cout <<resp;
            resp="";
            sock_m.send( "cucu\r\n");
            sock_m.recv( resp);
            cout <<resp;

            resp="";
            sock_m.send( "cucu");
            sock_m.recv( resp);
            cout <<resp;
      }
      catch( char* exception)
      {
            cout <<"Exception: " <<exception  <<endl;
      }
      return 1;
}
