#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include "debug.h"

using namespace std;

// script_name<tab>command<tab>error<tab>BTest_ID

Info::Info( void)
{
      RING;
      logFile =fopen( "error.log", "a");
      if( logFile == NULL)
      {
            throw (char*)"Can't open file";
      }
}



Info::~Info( void)
{
      RING;
      int retVal =fclose( logFile);
      if( retVal)
      {
            throw strerror( errno);
      }
}



void
Info::info( const char* prefix, const char* format, ...)
{
      RING;
      va_list ap;
      char infoStr[5000];
      va_start(ap, format);
      vsnprintf(infoStr, 5000, format, ap);
      va_end(ap);
      string color="\033[22;36m";
      string resetCOLOR ="\033[0m";

      if( ! strncasecmp( prefix, "recv", 4))
      {
            color ="\033[1;32m";
      }

/*            <<"ERR>> "
            <<where_cp
            <<"(\033[1;31m"
            <<errorMessage_cp
            <<"\033[0m)("
            <<"\033[22;31m"
            <<errStr
            <<"\033[0m)"
            <<endl;
*/
      cout  <<" OK<< "
            <<prefix
            <<"("
            <<color
            <<infoStr
            <<resetCOLOR
            <<")"
            <<endl;
}




void
Info::error( const char* where_cp, const char* errorMessage_cp, const char* reason_cp, ...)
{
      RING;
      va_list ap;
      char reasonStr[5000];

      va_start(ap, reason_cp);
            vsnprintf(reasonStr, 5000, reason_cp, ap);
      va_end(ap);

string COLOR1 ="\033[1;31m";
string COLOR2 ="\033[22;31m";
string resetCOLOR ="\033[0m";

      cerr
            <<"ERR>> "
            <<where_cp
            <<"("
            <<COLOR1
                  <<errorMessage_cp
            <<resetCOLOR
            <<")("
            <<COLOR2
                  <<reasonStr
            <<resetCOLOR
            <<")"
            <<endl;
            // script_name<tab>command<tab>error<tab>BTest_ID
            fprintf( logFile, "%s\t%s\t%s\t%s\n", runedScript.c_str(), where_cp, errorMessage_cp, reason_cp);
}

//Info info_m;
