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
inline static size_t mgets( char *line );

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



/**************************************************************************/


    inline static size_t
mgets( char *line )
{
    char *s = line;
    for( ; *line != '\n' && *line != 0; ++line )    /* 0.07% cache miss */
        ;

    *line = 0;
    return line - s;
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
    char *line, *p = NULL, *map, *end;
    int fd = -1, type = 0;
    int offset;
    struct stat statbuf;
    unsigned int chars_left = BUF_SZ;
    off_t file_ofst = 0, line_ofst=0;

    fd = open( fname, O_RDWR );

    if( fd < 0 ) {
        printf( "Unable to open '%s'\n", fname );
        exit( EXIT_FAILURE );
    }

    fstat( fd, &statbuf );
    map = mmap( 0, BUF_SZ * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE,
            fd, 0 );
    if( map == MAP_FAILED ) {
        printf( "mmap error for input\n" );
        exit( EXIT_FAILURE );
    }

    for( ; file_ofst < statbuf.st_size; ) {
        /* No more lines to read, then mmap some more */
        if( chars_left < LINE_LEN ) {
            file_ofst += BUF_SZ * 4096 - chars_left;
            munmap( map, BUF_SZ * 4096 );
            offset = file_ofst - ( file_ofst % PAGE_SZ );
            map = mmap( 0, BUF_SZ * PAGE_SZ, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE, fd, offset );
        }
        mgets( map + line_ofst );

        line = map + line_ofst ;

        chars_left -= mgets( line );
        dprintf( ( "left to read:%d\n", chars_left ) );

        dprintf( ( "%s\n", line ) );

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
