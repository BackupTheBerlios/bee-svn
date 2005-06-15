#include "socket.h"
#include <iostream>
using namespace std;
int main()
{
      try
      {
            Socket m_sock( AF_INET, SOCK_STREAM, 0);
      }
      catch( char* exception)
      {
            cout <<"Exception" <<exception <<"//" <<endl;
      }
      return 1;
}
