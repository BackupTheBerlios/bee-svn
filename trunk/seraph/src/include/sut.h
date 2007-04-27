#if !defined SUT_H
#define SUT_H

#include <glib.h>

bool
sut_refresh( const int test_type, const char *source, const char *dest, const char *host, const int port );
bool sut_start( const int test_type,
                const int timeout, const char* maillog,
                const char* start, const char* host, const int port);

bool sut_stop(  const int test_type,
                const int timeout, const char* maillog,
                const char* stop,const char* host,const int port);

bool sut_refresh(   const int test_type,
                    const char* source, const char* dest,
                    const char* host, const int port);
void sut_sigpipe(int);
void sut_sigint(int);

#else
        #warning "*** Header allready included ***"
#endif
