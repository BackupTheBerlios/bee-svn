#ifndef _SUT_PRIVATE_H_
#define _SUT_PRIVATE_H_
static bool sut_startRemote( const int timeout, const char *maillog, const char *start,
			     const char *host, const int port );

static bool sut_startLocal( const int timeout, const char *maillog, const char *start );

static bool sut_stopRemote( const int timeout, const char *maillog, const char *stop,
			    const char *host, const int port );

static bool sut_stopLocal( const int timeout, const char *maillog, const char *stop );

static bool sut_refreshRemote( const char *host, const int port, const char* refreshCmd );

static bool sut_refreshLocal( const char *refreshCmd);

static int sut_sutRefresh( const int option, const char *filename );
#endif
