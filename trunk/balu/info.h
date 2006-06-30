#include "debug.h"
#include <string>

class Info {

      public:
            FILE* m_log;

      public:
            Info( void);
           ~Info( void);
            void  info ( const char* function, const char* infoMessage, ...);
            void  error( const char* fileName_cp, const char* where_cp, const char* errorMessage_cp, const char* reason_cp, ...);
};

