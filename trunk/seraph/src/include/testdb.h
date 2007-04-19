#if !defined TESTDB_H
#define TESTDB_H
#include <glib.h>

typedef struct {
    char* symbol;
    char* value;
} ConfigEntry ;

char**  testdb_listTests    (const char* td, int* sz) ;
char**  testdb_listMachines (const char* td, int* sz) ;
GSList* testdb_getConfig    (const char* machine, int* sz) ;
bool    testdb_addMachine   (const char* mName, const char* mOS,
                             const char* mOSVer, const char* mIP);
#else
        #warning "*** Header allready included ***"
#endif
