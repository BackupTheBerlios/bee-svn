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


#define IS_NEW  1
#define IS_NEWA 2

#define IS_DEL  3
#define IS_DELA 4

#define LINE_LEN 1024
#define BUF_SZ   256
#define PAGE_SZ  4096
#define FNAME_LEN 256

#ifdef DEBUG
#define dprintf(a) fprintf a
#else
#define dprintf(a) ;
#endif

/* Keeps meta-data one new().
 * Uses only 4 bytes to fit in one %movsw */
/* Test if BITWISE operation is faster
 * than bitfields */
typedef struct {
        unsigned short int line:15;     /* Line where was allocated */
        unsigned short int is_new:1;    /* new() operator */
        unsigned short int fid:15;      /* file id */
        unsigned short int is_newa:1;   /* new[] operator */
} nod_t;


inline static int    parseLine( dict_ptr dict, const char text[], nod_t * res, int *type );

inline static void   mtrace( const char *const fname );

inline static size_t mgets( char *start, char **end );

inline static void   readSzFile( const char *text, int *sz, char *file, int fileLen );

inline static void   readInt( const char *text, int *res );

int  runTestSuite(void) ;
void test_mgets(void);
void test_readInt(void);

inline void handleType(int type) ;
int init_suite1(void){return 0;}
int clean_suite1(void){return 0;}
#endif
