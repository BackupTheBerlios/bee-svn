/*
 * gcc -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops mmtrace.c -o mtrace
 *
 */
#include "mmtrace.h"
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>


typedef struct {
        char *b_line;
        char *b_map;
        int b_pageOffset;
        off_t b_fileOffset;
        unsigned int b_chars;
} buffer_t;

#define PRINT_INFO \
                dprintf( ( stderr,\
                           "pageOffset=%d filePos=%jd fileSz=%jd charsInBuf=%d tmp=%d\n",\
                           bp.b_pageOffset, \
                           ( intmax_t ) bp.b_fileOffset, ( intmax_t ) statbuf.st_size,\
                           bp.b_chars , tmp) );\
                dprintf( ( stderr, "[%d][%s]\n", bp.b_line[0], bp.b_line ) );

/*
 * To overcome the memory consumption, the tables T1&T2 can be dumped
 * to disk.
 */

char buf[1000] = { 0 };
void test_bread(  );

/*------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
        if( argc < 3 )
                err( "Usage: mleak debug.log test?\n" );

        sprintf( buf, "cat /proc/%d/status >>stats", getpid(  ) );
        if( argv[2][0] != 't' )
        {
                mtrace( argv[1] );
                system( buf );
                return 0;
        }

        runTestSuite(  );
        return 0;
}

/*........................................*/


/*------------------------------------------------------------------------*/
inline static size_t mgets( char *start, char **end )
{
        char *s = start;
        size_t r = 0;

        if( !start || !*start )
        {
                printf( "RET 0\n" );
                return 0;
        }

        /* 0.07% cache * miss */
        for( ; *start != '\n' && *start != 0; ++start )
                ;
        r = ( *start == 0 ) ? start - s : start - s + 1;
        *start = 0;
        *end = start + 1;
        return r;
}

/*........................................*/


/*------------------------------------------------------------------------*/
inline static void readInt( const char *text, int *sz )
{
        for( *sz = 0;
             *text != '\0' && *text != ' ' &&
             *text >= '0' && *text <= '9'; ++text )
                *sz = ( *sz ) * 10 + *text - '0';
}

/*........................................*/


/*------------------------------------------------------------------------*/
inline static void
readSzFile( const char *text, int *sz, char *file, int fileLen )
{
        int i = 0;

        for( *sz = 0; *text != '\0' && *text != ' '; ++text )
                *sz = ( *sz ) * 10 + *text - '0';

        if( *text == '\0' )
        {
                file[0] = '\0';
                return;
        }

        for( i = 0; *text != '\0'; ++i, ++text );

        /* 0.1 % cache misses */
        memcpy( file, text + 1, i );
}

/*........................................*/


/*------------------------------------------------------------------------*/
inline void checkAddress( int type, int ptr, nod_t nod, dict_ptr dict )
{
        nod_t A;
        int found = 0;

        switch ( type )
        {
        case IS_NEW:
                //dprintf( ("---new()--\n") );
                A.line = nod.line;
                A.fid = 1;
                A.is_new = 1;
                insert( dict, ptr, A );
                break;

        case IS_NEWA:
                //dprintf( "---new[]--\n" );
                A.line = nod.line;
                A.fid = 1;
                A.is_newa = 1;
                insert( dict, ptr, A );
                break;

        case IS_DEL:
                //dprintf( "---delete()--\n" );
                found = lookup( dict, ptr );
                if( !found )
                        printf( "Double free\n" );
                delete( dict, ptr );
                break;

        case IS_DELA:
                //dprintf( "---delete[]--\n" );
                if( !found )
                        printf( "Double free\n" );
                delete( dict, ptr );
                break;
        default:
                //dprintf( "Unknown operator\n" );
                break;
        }
}

/*........................................*/


/*------------------------------------------------------------------------*/
inline static int parseLine( const char *const text, nod_t * res, int *type )
{
        char op[8];             /* operator ( new or delete ) */
        char *p;                /* find line Number */
        int sz = 0, line = 0, ptr;
        char file[FNAME_LEN] = { 0 };

        sscanf( text, "%s %x", op, &ptr );

        switch ( op[0] - op[4] )
        {
        case IS_NEW:
                res->is_new = 1;
                *type = IS_NEW;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
                break;
        case IS_NEWA:
                res->is_newa = 1;
                *type = IS_NEWA;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
                break;
        case IS_DEL:
                *type = IS_DEL;
                memcpy( file, text + 16, FNAME_LEN );
                break;
        case IS_DELA:
                *type = IS_DELA;
                memcpy( file, text + 16, FNAME_LEN );
                break;
        default:
                return 0;
                break;
        }
        p = memchr( file, '(', FNAME_LEN );

        if( !p )
                return 0;
        *p = 0;
        readInt( p + 1, &line );

        res->line = line;
        return ptr;
}

/*........................................*/



/*------------------------------------------------------------------------*/
inline static void mtrace( const char *const fname )
{
        char *p = NULL, *end = 0;
        int fd = -1;
        struct stat statbuf;
        dict_ptr dict;
        buffer_t bp = {.b_line = 0,.b_map = 0,.b_pageOffset = 0,.b_fileOffset =
                    0,.b_chars = 0
        };

        if( ( fd = open( fname, O_RDWR ) ) < 0 )
                err( "Unable to debug file\n" );

        dict = construct_dict( MIN_DICT_SIZE );
        fstat( fd, &statbuf );

        bread( fd, &bp, 0 );

        for( ; bp.b_fileOffset <= statbuf.st_size; bp.b_line = end )
        {
                int ptr = 0, tmp = 0, type = 0;
                nod_t nod;

                if( bp.b_chars < LINE_LEN )
                {
                        printf( "REMAP\n" );
                        bread( fd, &bp, 0 );
                }

                tmp = mgets( bp.b_line, &end );
                if( !tmp )
                        continue;
                bp.b_chars -= tmp;
                PRINT_INFO;

                if( bp.b_line[0] != 'M' && bp.b_line[6] != 'O' )
                        continue;
                p = bp.b_line + 9;      /* Advance over 'MEMINFO: ' */

                ptr = parseLine( p, &nod, &type );
                checkAddress( type, ptr, nod, dict );
        }
        close( fd );
        destruct_dict( dict );
}

/*........................................*/


/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
void test_mgets( void )
{
        char *end;
        char text[1024] = "abc\n";

        CU_ASSERT( mgets( text, &end ) == 4 );
        strcpy( text, "\n" );
        CU_ASSERT( mgets( text, &end ) == 1 );
        strcpy( text, "\0\n" );
        CU_ASSERT( mgets( text, &end ) == 0 );
        memset( text, 'a', 1023 );
        text[1023] = 0;
        CU_ASSERT( mgets( text, &end ) == 1023 );
}

/*........................................*/


/*------------------------------------------------------------------------*/
void test_readInt(  )
{
        int end = 0;
        char text[1024] = { 0 };

        strcpy( text, "127" );
        readInt( text, &end );
        CU_ASSERT( end == 127 );

        strcpy( text, "abc" );
        readInt( text, &end );
        CU_ASSERT( end == 0 );

        strcpy( text, "\0" );
        readInt( text, &end );
        CU_ASSERT( end == 0 );

        strcpy( text, "10" );
        readInt( text, &end );
        CU_ASSERT( end == 10 );

        strcpy( text, "99" );
        readInt( text, &end );
        CU_ASSERT( end == 99 );

        strcpy( text, "99\n" );
        readInt( text, &end );
        CU_ASSERT( end == 99 );

        strcpy( text, "99 " );
        readInt( text, &end );
        CU_ASSERT( end == 99 );
}

/*........................................*/


/*------------------------------------------------------------------------*/
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

/**********************************
 * Format for new:
 * MEMINFO(("new() " PTR_FORMAT " %d %s(%d){%d, %d}", p, sz, dd.file, dd.line, getpid(), Thread::getCurrentThreadId()));
 *
 * Reading dataTypes from a char* with sscanf is highly inefficient
 * for two reasons :
 *  1. adds cache misses
 *  2. call chain that goes like: sscanf -> vsscanf -> vfscanf
 */
