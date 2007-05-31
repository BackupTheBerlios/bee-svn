#if !defined TESTDB_H
#define TESTDB_H
#include <glib.h>

typedef struct {
    char* symbol;
    char* value;
} ConfigEntry ;

char**  testdb_listTests    (const char* td, int* sz) ;
char**  testdb_listUserTests(const char* uName, int* sz) ;
char**  testdb_listMachines (const char* td, int* sz) ;
bool    testdb_clearRunnedJobs(const char* uName) ;
GSList* testdb_getConfig    (const char* uName, const char* machine, unsigned int* sz) ;
bool    testdb_addMachine   (const char* uName, const char* mName, const char* mOS,
                             const char* mOSVer, const char* mIP);
bool    testdb_addUserTest  (const char* uName, int fSize, const char* fData);


bool
testdb_register( const char* const name, const char* const email,
                const char* const username, const char* const password) ;

bool
testdb_checkSession(const char* const username,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip ) ;

bool
testdb_setSession(  const char* const id,
                    const char* const session,
                    const char* const ip);

bool
testdb_checkLogin(  const char* const username,
                    const char* const password);

bool
testdb_checkRemembered( const char* const username,
                        const char* const cookie ) ;
#else
        #warning "*** Header allready included ***"
#endif
