#include "socket.h"
#include <iostream>
using namespace std;
int main()
{
      try
      {
            Socket m_sock;
      }
      catch( char* exception)
      {
            cout <<"Exception" <<exception <<"//" <<endl;
      }
      return 1;
}
