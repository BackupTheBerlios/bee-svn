#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include "info.h"

using namespace std;

// script_name<tab>command<tab>error<tab>BTest_ID

Info::Info( void)
{
      RING;
      m_log =fopen( "error.log", "w+");
      if( m_log == NULL)
      {
            throw (char*)"Can't open file";
      }
}



Info::~Info( void)
{
      RING;
      int retVal =fclose( m_log);
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
      string color="(\033[22;36m";

      if( ! strncasecmp( prefix, "recv", 4))
      {
            color ="\033[1;32m";
      }
      cout  <<" OK<< "
            <<prefix
            <<color
            <<infoStr
            <<"\033[0m"
            <<")"
            <<endl;
}




void
Info::error(const char* fileName_cp, const char* where_cp, const char* errorMessage_cp, const char* reason_cp, ...)
{
      RING;
      va_list ap;
      char errStr[5000];

      va_start(ap, reason_cp);
            vsnprintf(errStr, 5000, reason_cp, ap);
      va_end(ap);

      cerr
            <<"ERR>> "
            <<where_cp
            <<"(\033[1;31m"
            <<errorMessage_cp
            <<"\033[0m)("
            <<"\033[22;31m"
            <<errStr
            <<"\033[0m)"
            <<endl;
            // script_name<tab>command<tab>error<tab>BTest_ID
            fprintf( m_log, "%s\t%s\t%s\t%s\n", fileName_cp, where_cp, errorMessage_cp, reason_cp);
}
