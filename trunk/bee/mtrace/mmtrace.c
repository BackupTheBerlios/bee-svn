/*
 * gcc -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops mmtrace.c -o mtrace
 *
 */
#include "mmtrace.h"
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>

#define err(a) {fprintf(stderr, a);\
        exit(EXIT_FAILURE);}

                char buf[1000]={0};
int
main( int argc, char *argv[] )
{
        struct mallinfo mi;
        if( argc < 3 )
                err( "Usage: mleak debug.log test?\n" );

                sprintf( buf, "cat /proc/%d/status >>stats", getpid() );
        if( argv[2][0] != 't' )
        {
                mtrace( argv[1] );
                //mallinfo();
                system(buf);
                return 0;
        }

        runTestSuite(  );
        return 0;
}



/*------------------------------------------------------------------------*/
int
runTestSuite( void )
{
        CU_pSuite       pSuite = NULL;

         /* initialize the CUnit test registry*/
        if( CUE_SUCCESS != CU_initialize_registry(  ) )
                return CU_get_error(  );

         /* add a suite to the registry*/
        pSuite = CU_add_suite( "Suite_1", init_suite1, clean_suite1 );
        if( NULL == pSuite )
        {
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

         /* add the tests to the suite */
        if( ( NULL == CU_add_test( pSuite, "mgets()", test_mgets ) ) ||
            ( NULL == CU_add_test( pSuite, "readInt()", test_readInt ) ) )
        {
                printf( "Error\n" );
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

        /* Run all tests using the CUnit Basic interface */
        CU_basic_set_mode( CU_BRM_VERBOSE );
        CU_basic_run_tests(  );
        CU_cleanup_registry(  );
        return CU_get_error(  );
}


/*------------------------------------------------------------------------*/
inline static   size_t
mgets( char *start, char **end )
{
        char           *s = start;
        size_t          r = 0;

        if( !start || !*start )
        {
                printf( "RET 0\n" );
                return 0;
        }

        for( ; *start != '\n' && *start != 0; ++start ) /* 0.07% cache * miss */
                ;
        r = ( *start == 0 ) ? start - s : start - s + 1;
        *start = 0;
        *end = start + 1;
        return r;
}


void
test_mgets( void )
{
        char           *end;
        char            text[1024] = "abc\n";

        CU_ASSERT( mgets( text, &end ) == 4 );
        strcpy( text, "\n" );
        CU_ASSERT( mgets( text, &end ) == 1 );
        strcpy( text, "\0\n" );
        CU_ASSERT( mgets( text, &end ) == 0 );
        memset( text, 'a', 1023 );
        text[1023] = 0;
        CU_ASSERT( mgets( text, &end ) == 1023 );
}


/*------------------------------------------------------------------------*/
inline static void
readInt( const char *text, int *sz )
{
        for( *sz = 0;
             *text != '\0' && *text != ' ' &&
             *text >= '0' && *text <= '9'; ++text )
                *sz = ( *sz ) * 10 + *text - '0';
}

void
test_readInt(  )
{
        int             end = 0;
        char            text[1024] = { 0 };

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


/*------------------------------------------------------------------------*/
inline static void
readSzFile( const char *text, int *sz, char *file, int fileLen )
{
        int             i = 0;

        for( *sz = 0; *text != '\0' && *text != ' '; ++text )
                *sz = ( *sz ) * 10 + *text - '0';

        if( *text == '\0' )
        {
                file[0] = '\0';
                return;
        }

        for( i = 0; *text != '\0'; ++i, ++text );

        memcpy( file, text + 1, i );    /* 0.1 % cache misses */
}



/*------------------------------------------------------------------------*/
inline static void
mtrace( const char *const fname )
{
        nod_t           nod;
        char           *line = 0,
            *p = NULL,
            *map = 0,
            *end = 0;
        int             fd = -1,
            type = 0;
        int             pageOffset = 0,
            ptr = 0;
        struct stat     statbuf;
        off_t           filePos = 0,
            lastPageOffset = 0;
        unsigned int    charsInBuf = 0;
        dict_ptr        dictionary;
        int             tmp;
        int             min_size = 1024;


        if( ( fd = open( fname, O_RDWR ) ) < 0 )
                err( "Unable to debug file");

        dictionary = construct_dict( min_size );
        fstat( fd, &statbuf );

        map = mmap( 0, BUF_SZ * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0 );
        if( map == MAP_FAILED ) err( "mmap error for input\n" );

        filePos = BUF_SZ * PAGE_SZ;
        pageOffset = filePos - ( filePos % PAGE_SZ );
        charsInBuf = PAGE_SZ * BUF_SZ;
        line = map;

        for( ; filePos <= statbuf.st_size; line = end )
        {
                 /* buffer has less than a line */
                if( charsInBuf < LINE_LEN )
                {
                        dprintf( ( stderr, "REMAP\n" ) );
                        munmap( map, BUF_SZ * PAGE_SZ );

                        map =
                            mmap( 0, BUF_SZ * PAGE_SZ,
                                  PROT_READ | PROT_WRITE, MAP_PRIVATE, fd,
                                  pageOffset );
                        if( map == MAP_FAILED )
                                err( "mmap error for input\n" );
                        filePos += BUF_SZ * PAGE_SZ - charsInBuf;
                        pageOffset = filePos - ( filePos % PAGE_SZ );
                        line = map + lastPageOffset;
                        charsInBuf = PAGE_SZ * BUF_SZ - charsInBuf;
                        system(buf);
                }

                tmp = mgets( line, &end );
                if( !tmp )
                        continue;
                charsInBuf -= tmp;

                dprintf( ( stderr,
                           "pageOffset=%d lastPageOffset=%jd filePos=%jd fileSz=%jd charsInBuf=%d\n",
                           pageOffset, ( intmax_t ) lastPageOffset,( intmax_t ) filePos,
                           ( intmax_t ) statbuf.st_size, charsInBuf ) );

                dprintf( ( stderr, "[%d][%s]\n", line[0], line ) );
                printf( "%s\n", line );
                if( line[0] != 'M' && line[6] != 'O' )
                        continue;
                p = line + 9;   /* Advance over 'MEMINFO: ' */

                ptr = parseLine( p, &nod, &type );
                checkAddress( type, ptr, nod, dictionary );
        }
        close( fd );
        destruct_dict( dictionary );
}


/*------------------------------------------------------------------------*/
inline void
checkAddress( int type, int ptr, nod_t nod, dict_ptr dict )
{
        nod_t          A ;
        int             found = 0;

        switch ( type )
        {
        case IS_NEW:
                dprintf( ( stderr, "---new()--\n" ) );
                //A = malloc( sizeof( nod_t ) );
                A.line = nod.line;
                A.fid = 1;
                A.is_new = 1;
                insert( dict, ptr, A );
                break;

        case IS_NEWA:
                dprintf( ( stderr, "---new[]--\n" ) );
                //A = malloc( sizeof( nod_t ) );
                A.line = nod.line;
                A.fid = 1;
                A.is_newa = 1;
                insert( dict, ptr, A );
                break;

        case IS_DEL:
                dprintf( ( stderr, "---delete()--\n" ) );
                found = lookup( dict, ptr );
                if( !found )
                        printf("Double free\n");
                delete( dict, ptr);
                break;

        case IS_DELA:
                dprintf( ( stderr, "---delete[]--\n" ) );
                if( !found )
                        printf("Double free\n");
                delete( dict, ptr);
                break;
        default:
                dprintf( ( stderr, "Unknown operator\n" ) );
                break;
        }
}

/*------------------------------------------------------------------------*/
inline static int
parseLine( const char *const text, nod_t * res, int *type )
{
        char            op[8];  /* operator ( new or delete ) */
        char           *p;      /* find line Number */
        int             sz = 0,
            line = 0,
            ptr;
        char            file[FNAME_LEN] = { 0 };

        sscanf( text, "%s %x", op, &ptr );

        if( op[0] == 'n' && op[4] == ')' )
        {
                res->is_new = 1;
                *type = IS_NEW;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
        } else if( op[0] == 'n' && op[4] == ']' )
        {
                res->is_newa = 1;
                *type = IS_NEWA;
                readSzFile( text + 16, &sz, file, FNAME_LEN );
        } else if( op[0] == 'd' && op[4] == ')' )
        {
                *type = IS_DEL;
                memcpy( file, text + 16, FNAME_LEN );
        } else if( op[0] == 'd' && op[4] == ']' )
        {
                *type = IS_DELA;
                memcpy( file, text + 16, FNAME_LEN );
        }

        p = memchr( file, '(', FNAME_LEN );

        if( !p )
                return 0;
        *p = 0;
        readInt( p + 1, &line );

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

/**********************************
 * Format for new:
 * MEMINFO(("new() " PTR_FORMAT " %d %s(%d){%d, %d}", p, sz, dd.file, dd.line, getpid(), Thread::getCurrentThreadId()));
 *
 * Reading dataTypes from a char* with sscanf is highly inefficient
 * for two reasons :
 *  1. adds cache misses
 *  2. call chain that goes like: sscanf -> vsscanf -> vfscanf
 */
