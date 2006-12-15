#if !defined( _MMTRACE_H_ )
#define _MMTRACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "cuckoo.h"

#define DETAIL 0

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


#define IS_NEW  69               /* op[0]='n', op[4]=')' */
#define IS_NEWA 93               /* op[0]='n', op[4]=']' */

#define IS_DEL  59               /* op[0]='d', op[4]=')' */
#define IS_DELA 7                /* op[0]='d', op[4]=']' */

#define PAGE_SZ     4096
#define BUF_SZ      256
#define LINE_LEN    1024
#define FNAME_LEN   256
#define MIN_DICT_SIZE 1024


#ifdef DEBUG
#define dprintf(a) fprintf a
#else
#define dprintf(a) ;
#endif

#define err(a) {fprintf(stderr, a); exit(EXIT_FAILURE);}

inline static int    parseLine( const char text[], nod_t * res, int *type );

inline static void   mtrace( const char *const fname );

inline static size_t mgets( char *start, char **end );

inline static void   readSzFile( const char *text, int *sz, char *file, int fileLen );

inline static void   readInt( const char *text, int *res );

int  runTestSuite(void) ;
void test_mgets(void);
void test_readInt(void);

inline void checkAddress(int type, int ptr, nod_t node, dict_ptr dict) ;
int init_suite1(void){return 0;}
int clean_suite1(void){return 0;}
#endif
