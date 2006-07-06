#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>
#include <iostream>
#include "debug.h"


#ifndef RING
      #define RING  if( getenv("DEBUG") !=NULL) cout<<__FILE__<<":\t"<<__LINE__<<"\t"<<__PRETTY_FUNCTION__<<endl;
#endif


class Info {

      public:
            FILE* logFile;
            std::string runedScript;

      public:
            Info( void);
           ~Info( void);
            void  info ( const char* function, const char* infoMessage, ...);
            void  error( const char* where_cp, const char* errorMessage_cp, const char* reason_cp, ...);

};

extern Info info_m;
#endif /* DEBUG_H */
