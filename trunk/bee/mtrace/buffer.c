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
         
void
test_bread();

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
             ( NULL == CU_add_test( pSuite, "rebuf()", test_bread ) )
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
mgets( char *start, char **end )
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
         *end = start + 1;
         return r;
}

/*------------------------------------------------------------------------*/

inline static char*
binit(int fd, buffer_t * bp, size_t size)
{
    if(size==0) {
        size = BUF_SZ * PAGE_SZ;
    }
    bp->b_chars = size;

    bp->b_map = mmap( 0, size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE, fd, 0 );

    if( bp->b_map == MAP_FAILED )
        err( "mmap error for inputF\n" );

    bp->b_fileOffset += size - bp->b_chars;
    bp->b_pageOffset = bp->b_fileOffset - ( bp->b_fileOffset % PAGE_SZ );
    bp->b_line = bp->b_map ;
    return bp->b_map;
}

inline static char *
bread( int fd, buffer_t * bp , size_t size)
{
        int small=1;
        if(!size) {
            size =BUF_SZ * PAGE_SZ;
            small = 0;
        }

         munmap( bp->b_map, size );
         fprintf( stderr, "fileOffset=%jd\n", (intmax_t)bp->b_fileOffset );

         bp->b_map = mmap( 0, size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE, fd, bp->b_pageOffset );

         if( bp->b_map == MAP_FAILED )
                  err( "mmap error for inputF\n" );

         bp->b_fileOffset += size - bp->b_chars;
         bp->b_pageOffset = bp->b_fileOffset - ( bp->b_fileOffset % PAGE_SZ );
         if( bp->b_chars >=4096)
         {
            printf("ERROR\n");
            exit(EXIT_FAILURE);
         }
        if(small)
            bp->b_line = bp->b_map ;
        else
            bp->b_line = bp->b_map + ( 4096 - bp->b_chars );
        bp->b_chars = size - bp->b_chars;
        return bp->b_map;
}

void test_bread_file(char* fname, int lines)
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
        size = statbuf.st_size ;
        /* In case st_size < 1Mb */
        if( size >= PAGE_SZ* BUF_SZ )
        {
            printf("Going big\n");
            size = 0;
        }
        binit(fd, &bp, size);
        while( bp.b_fileOffset <= statbuf.st_size )
        {   int tmp=0;

                do {
                    tmp = mgets( bp.b_line, &end );
                    ++linesRead ;
                    bp.b_chars -= tmp;
                    printf("%s\n", bp.b_line);
                    bp.b_line = end;
                }while(bp.b_chars > LINE_LEN && tmp);

                if(!tmp) {
                    CU_ASSERT( linesRead == lines );
                    return;
                }
                printf( "REMAP\n" );
                bread( fd, &bp, size );
                printf("LinesRead=%d should read=%d\n", linesRead, lines);
        }
        /*-----------*/
}


void
test_bread()
{
    test_bread_file("d1", 30+1);
    test_bread_file("d2", 30+1);
    test_bread_file("d3", 14007+1);
}
/*........................................*/

