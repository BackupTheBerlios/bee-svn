/*
 * gcc -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops mmtrace.c -o mtrace
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#define IS_NEW  1
#define IS_NEWA 2

#define IS_DEL  3
#define IS_DELA 4



#ifdef DEBUG
#define dprintf(a) printf a
#else
#define dprintf(a) ;
#endif


/**********************************
 * Format for new:
 * MEMINFO(("new() " PTR_FORMAT " %d %s(%d){%d, %d}", p, sz, dd.file, dd.line, getpid(), Thread::getCurrentThreadId()));
 *
 * Reading dataTypes from a char* with sscanf is highly inefficient
 * for two reasons :
 *  1. adds cache misses
 *  2. call chain that goes like: sscanf -> vsscanf -> vfscanf
 */

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


inline static int
parseLine( const char text[], nod_t * res, int *type );


inline static void
mtrace( const char *const fname );


/* The most called function */
inline static size_t mgets( char *start, char** end );

inline static void
readSzFile( const char *text, int *sz, char *file, int fileLen );

inline static void
readInt( const char *text, int *res );

        int
main( int argc, char *argv[] )
{
        if( argc < 2 ) {
                fprintf( stderr, "Usage: mleak debug.log\n" );
                exit( EXIT_FAILURE );
        }
        mtrace( argv[1] );
        return 0;
}



/*------------------------------------------------------------------------*/


        inline static size_t
mgets( char *start, char** end )
{
        char *s = start;
        for( ; *start != '\n' && *start != 0; ++start )    /* 0.07% cache miss */
                ;

        *start = 0;
        *end = start+1 ;
        return start - s ;
}

        inline static void
readInt( const char *text, int *sz )
{
        for( *sz = 0; *text != '\0' && *text != ' '; ++text )
                *sz = ( *sz ) * 10 + *text - '0';
}

        inline static void
readSzFile( const char *text, int *sz, char *file, int fileLen )
{
        int i = 0;
        for( *sz = 0; *text != '\0' && *text != ' '; ++text )
                *sz = ( *sz ) * 10 + *text - '0';

        if( *text == '\0' ) {
                file[0] = '\0';
                return;
        }

        for( i = 0; *text != '\0'; ++i, ++text );

        memcpy( file, text + 1, i );    /* 0.1 % cache misses */
}

#define LINE_LEN 1024
#define BUF_SZ   256
#define PAGE_SZ  4096
#define FNAME_LEN 256


        inline static void
mtrace( const char *const fname )
{
        nod_t nod;
        char *line=0, *p = NULL, *map=0, *end=0;
        int fd = -1, type = 0;
        int pageOffset=0;
        struct stat statbuf;
        unsigned int charsInBuf=0,r=0;
        off_t fileOffset = 0, lineOffset=0;

        if( (fd = open( fname, O_RDWR )) <0 ) {
                printf( "Unable to open '%s'\n", fname );
                exit( EXIT_FAILURE );
        }

        fstat( fd, &statbuf );

        for( ; fileOffset < statbuf.st_size ; line = end ) {
                /* No more lines to read, then mmap some more */
                if( charsInBuf < LINE_LEN ) {
                        fileOffset += BUF_SZ * PAGE_SZ - charsInBuf;
                        munmap( map, BUF_SZ * PAGE_SZ );
                        map = mmap( 0, BUF_SZ * PAGE_SZ, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE, fd, pageOffset );
                        if( map == MAP_FAILED ) {
                                printf( "mmap error for input\n" );
                                exit( EXIT_FAILURE );
                        }
                        if(charsInBuf) {
                                pageOffset = fileOffset - ( fileOffset % PAGE_SZ );
                                lineOffset = 4096 - charsInBuf ;
                        }
                        line = map + lineOffset ;
                        charsInBuf = PAGE_SZ*BUF_SZ ;
                }
                dprintf(( "pageOffset=%d lineOffset=%d fileOffset=%d charsInBuf=%d\n", pageOffset, lineOffset, fileOffset, charsInBuf));
                charsInBuf -= mgets( line, &end );
                dprintf( ( "[%s]\n", line ) );


                if( line[0] != 'M' && line[6] != 'O' )
                        continue;

                p = line + 9;    /* Advance over 'MEMINFO: ' */

                parseLine( p, &nod, &type );

                switch ( type ) {
                        case IS_NEW:
                                dprintf( ( "---new()--\n" ) );
                                break;

                        case IS_NEWA:
                                dprintf( ( "---new[]--\n" ) );
                                break;

                        case IS_DEL:
                                dprintf( ( "---delete()--\n" ) );
                                break;

                        case IS_DELA:
                                dprintf( ( "---delete[]--\n" ) );
                                break;
                        default:
                                dprintf( ( "Unknown operator\n" ) );
                                break;
                }
        }
        close( fd );
}



        inline static int
parseLine( const char *const text, nod_t * res, int *type )
{
        char op[8];             /* operator ( new or delete ) */
        char *p;                /* used to find line Number */
        int sz = 0, line = 0, ptr;
        char file[FNAME_LEN] = { 0 };   /* file */

        sscanf( text, "%s %x", op, &ptr );

        if( op[0] == 'n' && op[4] == ')' ) {
                dprintf( ( "+++new()++\n" ) );
                res->is_new = 1;
                *type = IS_NEW;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
        } else if( op[0] == 'n' && op[4] == ']' ) {
                dprintf( ( "+++new[]++\n" ) );
                res->is_newa = 1;
                *type = IS_NEWA;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
        } else if( op[0] == 'd' && op[4] == ')' ) {
                dprintf( ( "+++delete()++\n" ) );
                *type = IS_DEL;
                memcpy( file, text + 16, FNAME_LEN );
        } else if( op[0] == 'd' && op[4] == ']' ) {
                dprintf( ( "+++delete[]++\n" ) );
                *type = IS_DELA;
                memcpy( file, text + 16, FNAME_LEN );
        }

        p = memchr( file, '(', FNAME_LEN );

        if( !p )
                return 0;
        *p = 0;
        //sscanf( p + 1, "%d", &line );
        readInt( p + 1, &line );
        dprintf( ( "OP=%s HEX=%x SZ=%d FILE=%s LINE=%d\n", op, ptr, sz, file,
                                line ) );

        res->line = line;
        return ptr;
}



/*
 * Removing one sscanf call reduces the cache misses from:
 * ==5544== I   refs:      68,834,587
 * ==5544== I1  misses:           706
 * ==5544== L2i misses:           704
 * ==5544== I1  miss rate:       0.00%
 * ==5544== L2i miss rate:       0.00%
 * ==5544==
 * ==5544== D   refs:      43,840,799  (28,898,676 rd + 14,942,123 wr)
 * ==5544== D1  misses:        16,721  (    16,537 rd +        184 wr)
 * ==5544== L2d misses:        16,598  (    16,437 rd +        161 wr)
 * ==5544== D1  miss rate:        0.0% (       0.0%   +        0.0%  )
 * ==5544== L2d miss rate:        0.0% (       0.0%   +        0.0%  )
 * ==5544==
 * ==5544== L2 refs:           17,427  (    17,243 rd +        184 wr)
 * ==5544== L2 misses:         17,302  (    17,141 rd +        161 wr)
 * ==5544== L2 miss rate:         0.0% (       0.0%   +        0.0%  )
 *
 * TO
 *
 * ==5556== I   refs:      55,787,371
 * ==5556== I1  misses:           688
 * ==5556== L2i misses:           686
 * ==5556== I1  miss rate:       0.00%
 * ==5556== L2i miss rate:       0.00%
 * ==5556==
 * ==5556== D   refs:      36,195,007  (24,619,436 rd + 11,575,571 wr)
 * ==5556== D1  misses:        18,074  (    17,725 rd +        349 wr)
 * ==5556== L2d misses:        16,606  (    16,442 rd +        164 wr)
 * ==5556== D1  miss rate:        0.0% (       0.0%   +        0.0%  )
 * ==5556== L2d miss rate:        0.0% (       0.0%   +        0.0%  )
 * ==5556==
 * ==5556== L2 refs:           18,762  (    18,413 rd +        349 wr)
 * ==5556== L2 misses:         17,292  (    17,128 rd +        164 wr)
 * ==5556== L2 miss rate:         0.0% (       0.0%   +        0.0%  )
 */
