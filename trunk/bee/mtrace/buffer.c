/*
 *  indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c
 */

#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


typedef struct buffer_st {
        int pageOffset;
        int chunk_diff;
        int chunk_size;
        int fd;
        char *map;
        char *line;
        char *chunk_end;
} buffer_t;

extern void *memrchr( const void *s, int c, size_t n );
int init_suite1( void )
{
        return 0;
}
int clean_suite1( void )
{
        return 0;
}

#define err(a) {fprintf(stderr, a); exit(EXIT_FAILURE);}


inline static char *buf_init( int fd, buffer_t * bp, size_t size )
{
        bp->pageOffset = 0;
        bp->chunk_diff = 0;
        bp->chunk_size = 4096 * 256;
        bp->fd = fd;
        bp->map = 0;
        bp->line = 0;
        bp->chunk_end = 0;
        return 0;
}

void buf_show( buffer_t * bp )
{
        printf
            ( "\npageOffset:%d chunk_diff:%d chunk_size:%d fd:%d map:%p line:%p end:%p\n",
              bp->pageOffset, bp->chunk_diff, bp->chunk_size, bp->fd, bp->map,
              bp->line, bp->chunk_end );

}

void test_buf_rebuf(  );

int runTestSuite( void )
{
        CU_pSuite pSuite = NULL;

        if( CUE_SUCCESS != CU_initialize_registry(  ) )
                return CU_get_error(  );

        pSuite = CU_add_suite( "Suite_1", init_suite1, clean_suite1 );
        if( NULL == pSuite )
        {
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

        if( ( NULL == CU_add_test( pSuite, "rebuf()", test_buf_rebuf ) ) )
        {
                printf( "Error\n" );
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

        CU_basic_set_mode( CU_BRM_VERBOSE );
        CU_basic_run_tests(  );
        CU_cleanup_registry(  );
        return CU_get_error(  );
}

/*........................................*/
int main(  )
{
        runTestSuite(  );
        return 0;
}

inline static size_t buf_readline( char *start, char **end )
{
        char *s = start;
        size_t r = 0;

        if( !start || !*start )
        {
                printf( "RET 0\n" );
                return 0;
        }

        /* 0.07% cache * miss */
        while( *start != '\n' && *start != 0 )
        {
                ++start;
        }

        r = ( *start == 0 ) ? start - s : start - s + 1;
        *start = 0;
        *end = start + 1;
        return r;
}

inline static char *
buf_rebuf( buffer_t * bp, size_t chunk_size, char small )
{
        int prot = PROT_WRITE | PROT_READ;
        int flags = MAP_PRIVATE;

        if( -1 == munmap( bp->map, chunk_size ) )
                err( "munmap failed\n" );

        bp->map = mmap( 0, chunk_size,
                        prot, flags, bp->fd, bp->pageOffset );
        if( bp->map == MAP_FAILED )
                err( "mmap error for input\n" );

        bp->line = bp->map + bp->chunk_diff;

        if(small)
            bp->chunk_end = bp->map + chunk_size;
        else
            bp->chunk_end = memrchr( bp->map, '\n', chunk_size );

        bp->pageOffset += chunk_size - 4096;
        bp->chunk_diff =
            ( char * )bp->chunk_end - ( bp->map + ( chunk_size - 4096 ) ) ;

        return bp->map;
}
/*------------------------------------------------------------------------*/
#if 0
inline static char *buf_rebuf( buffer_t * bp, size_t size )
{
        int prot = PROT_WRITE | PROT_READ;
        int flags = MAP_PRIVATE;
        //printf( "rebuffing\n" );
        if( -1 == munmap( bp->map, bp->chunk_size ) )
                err( "munmap failed\n" );
        /* 0. compute new pageOffset, based on the
         * number of characters consumed from the old mapped */

        /* 1. advance pageOffset, so we can map new area */
        //buf_show( bp );
        bp->map = mmap( 0, bp->chunk_size, prot, flags,
                        bp->fd, bp->pageOffset );
        if( bp->map == MAP_FAILED )
                err( "mmap error for input\n" );

        /* 2. update the line */
        bp->line = bp->map + bp->chunk_diff;
        bp->pageOffset += bp->chunk_size - 4096;
        bp->chunk_end = memrchr( bp->map, '\n', bp->chunk_size );
        bp->chunk_diff =
            ( char * )bp->chunk_end - ( bp->map + ( bp->chunk_size - 4096 ) ) ;

        //buf_show( bp );
        //printf( "exit rebuffing\n" );
        return bp->map;
}
#endif
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*------------------------------------------------------------------------*/
void test_buf_rebuf_file( char *fname, int lines )
{
        struct stat statbuf;
        int fd;
        char *end;
        size_t linesRead = 0;
        size_t size = 0;
        buffer_t bp;

        if( ( fd = open( fname, O_RDWR ) ) < 0 )
                err( "Unable to debug file" );

        fstat( fd, &statbuf );
        size = statbuf.st_size;

        buf_init( fd, &bp, size );
        int tmp = 1;
        int left_to_read = 0;
//        int prot = PROT_WRITE | PROT_READ;
//        int flags = MAP_PRIVATE;

        /* big file */
        if( statbuf.st_size >= 10240 )
                while( bp.pageOffset <
                       ( size - size % bp.chunk_size -
                         ( size / bp.chunk_size ) * 4096 ) )
                {
                        buf_rebuf( &bp, bp.chunk_size,0 );
                        while( bp.line < bp.chunk_end )
                        {
                                tmp = buf_readline( bp.line, &end );
                                ++linesRead;
                                printf( "%s\n", bp.line );
                                bp.line = end;
                        }

                        if( !tmp )
                        {
                                CU_ASSERT( linesRead == lines );
                                printf( "LinesRead=%d should read=%d\n",
                                        linesRead, lines );
                                return;
                        }
                }

        /* st_size < 1Mb */
        //printf( "rebuffing:small\n" );
        left_to_read = statbuf.st_size - bp.pageOffset;
        buf_rebuf( &bp, left_to_read, 1);
#if 0
        if( -1 == munmap( bp.map, bp.chunk_size ) )
                err( "munmap failed\n" );
        bp.map = mmap( 0, left_to_read, prot, flags, bp.fd, bp.pageOffset );
        if( bp.map == MAP_FAILED )
                err( "mmap error for input\n" );

        /* 2. update the line */
        bp.line = bp.map + bp.chunk_diff + 1;
        bp.chunk_end = bp.map + left_to_read;
        if( bp.chunk_end )
                *( bp.chunk_end ) = '\n';
#endif
        while( bp.line != bp.chunk_end )
        {
                tmp = buf_readline( bp.line, &end );
                ++linesRead;
                printf( "%s\n", bp.line );
                bp.line = end;
        }

        CU_ASSERT( linesRead == lines );
        printf( "LinesRead=%d should read=%d\n", linesRead, lines );
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*------------------------------------------------------------------------*/
void test_buf_rebuf(  )
{
        test_buf_rebuf_file( "d1", 30 );
        test_buf_rebuf_file( "d2", 30 );
//        test_buf_rebuf_file( "d3", 28011 );
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
