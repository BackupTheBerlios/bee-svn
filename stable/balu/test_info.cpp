#include "info.h"
#include <iostream>
using namespace std;
int main(int argc, char** argv)
{
      try
      {
            Info log;
            log.info( "lmatch","found at %i",2);
            log.error( argv[0], "lmatch","found at %i","why",2);
      }
      catch( char* exception)
      {
            cout <<"Exception: " <<exception  <<endl;
      }
      return 1;
}
