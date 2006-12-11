#include "mmtrace.h"
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>


typedef struct {
         char *line;
         char *map;
         char *end;
         int  diff;
         int pageOffset;
         int fd;
         unsigned int chars;
} buffer_t;

inline static char*
buf_init(int fd, buffer_t * bp, size_t size)
{
    bp->line = 0 ;
    bp->map = 0 ;
    bp->pageOffset = 0;
    bp->fd = fd ;
    bp->chars = 0 ;
    return 0 ;
}

void buf_show(buffer_t* bp )
{
    printf("%s %d %d %d %d\n",
    bp->line,
    bp->map,
    bp->pageOffset,
    bp->fd ,
    bp->chars);
}
void
test_buf_rebuf();

int
runTestSuite( void )
{
         CU_pSuite pSuite = NULL;

         if( CUE_SUCCESS != CU_initialize_registry(  ) )
                  return CU_get_error(  );

         pSuite = CU_add_suite( "Suite_1", init_suite1, clean_suite1 );
         if( NULL == pSuite )
         {        CU_cleanup_registry(  );
                  return CU_get_error(  );
         }

         if( 
             ( NULL == CU_add_test( pSuite, "rebuf()", test_buf_rebuf ) )
           )
         {        printf( "Error\n" );
                  CU_cleanup_registry(  );
                  return CU_get_error(  );
         }

         CU_basic_set_mode( CU_BRM_VERBOSE );
         CU_basic_run_tests(  );
         CU_cleanup_registry(  );
         return CU_get_error(  );
}

/*........................................*/
int main() {
    runTestSuite(  );
    return 0;
}

inline static size_t
buf_readline( char *start, char **end )
{
         char *s = start;
         size_t r = 0;

         if( !start || !*start )
         {        printf( "RET 0\n" );
                  return 0;
         }

         /* 0.07% cache * miss */
         for( ; *start != '\n' && *start != 0; ++start )
                  ;
         r = ( *start == 0 ) ? start - s : start - s + 1;
         *start = 0;
         *end = start +1;
         return r;
}

/*------------------------------------------------------------------------*/

inline static char *
buf_rebuf( buffer_t * bp , size_t size)
{
    int idx ;
    munmap( bp->map, size );
    /* 0. compute new pageOffset, based on the
     * number of characters consumed from the old mapped */

    /* 1. advance pageOffset, so we can map new area */
    bp->map = mmap( bp->pageOffset );
    if( bp->map == MAP_FAILED )
        err( "mmap error for input\n" );

    /* 2. update the line */
    bp->line = bp->map + bp->diff;
    bp->end = memrchr(bp->map+PAGE_SZ*BUF_SZ, '\n', 4096);
    bp->diff = bp->map - bp->end;

    return bp->map;
}

void test_buf_rebuf_file(char* fname, int lines)
{
        struct stat statbuf;
        int fd;
        char *end;
        size_t linesRead=0;
        size_t size =0;
        buffer_t bp ;

        if( ( fd = open( fname, O_RDWR ) ) < 0 )
                err( "Unable to debug file" );

        fstat( fd, &statbuf );
        /* In case st_size < 1Mb */
        if( statbuf.st_size >= PAGE_SZ* BUF_SZ )
        {
            size = PAGE_SZ*BUF_SZ;
        }else
            size = statbuf.st_size ;

        buf_init(fd, &bp, size);
        while( bp.fileOffset <= statbuf.st_size )
        {   int tmp=0;

                do {
                    tmp = buf_readline( bp.line, &end );
                    ++linesRead ;
                    printf("%s\n", bp.line);
                    bp.line = end;
                }while(bp.line-1 !=bp.end && tmp);

                if(!tmp) {
                    CU_ASSERT( linesRead == lines );
                    return;
                }
                printf( "REMAP\n" );
                buf_rebuf( &bp, size );
                printf("LinesRead=%d should read=%d\n", linesRead, lines);
        }
        /*-----------*/
}


void
test_buf_rebuf()
{
    test_buf_rebuf_file("d1", 30+1);
    test_buf_rebuf_file("d2", 30+1);
    test_buf_rebuf_file("d3", 14007+1);
}
/*........................................*/

