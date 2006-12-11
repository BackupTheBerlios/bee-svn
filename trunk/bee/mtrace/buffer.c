#include "mmtrace.h"
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>


typedef struct {
         char *b_line;
         char *b_map;
         char *b_end;
         int b_pageOffset;
         int fd;
         unsigned int b_chars;
} buffer_t;

inline static char*
buf_init(int fd, buffer_t * bp, size_t size)
{
    bp->b_line = 0 ;
    bp->b_map = 0 ;
    bp->b_pageOffset = 0;
    bp->fd = fd ;
    bp->b_chars = 0 ;
    return 0 ;
}

void buf_show(buffer_t* bp )
{
    printf("%s %d %d %d %d\n",
    bp->b_line,
    bp->b_map,
    bp->b_pageOffset,
    bp->fd ,
    bp->b_chars);
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
         *end = start ;
         return r;
}

/*------------------------------------------------------------------------*/



inline static char *
buf_rebuf( buffer_t * bp , size_t size)
{
    munmap( bp->b_map, size );
    /* 0. compute new pageOffset, based on the
     * number of characters consumed from the old mapped */

    /* 1. advance pageOffset, so we can map new area */
    bp->b_map = mmap( bp->b_pageOffset );
    if( bp->b_map == MAP_FAILED )
        err( "mmap error for input\n" );
    /* 2. update the line */
    bp->b_line = ;
    bp->b_end = reverse_search( bp->b_map, mapped_size, "\n");
    return bp->b_map;
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
        while( bp.b_fileOffset <= statbuf.st_size )
        {   int tmp=0;

                do {
                    tmp = buf_readline( bp.b_line, &end );
                    ++linesRead ;
                    bp.b_chars -= tmp;
                    printf("%s\n", bp.b_line);
                    bp.b_line = end+1;
                }while(bp.b_line !=bp.b_end+1 && tmp);

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

