#if !defined( _MEMTRACE_H_ )
#define _MEMTRACE_H_

#define IS_NEW  69               /* op[0]='n', op[4]=')' */
#define IS_NEWA 93               /* op[0]='n', op[4]=']' */

#define IS_DEL  59               /* op[0]='d', op[4]=')' */
#define IS_DELA 7                /* op[0]='d', op[4]=']' */

#define PAGE_SZ     4096
#define BUF_SZ      256
#define LINE_LEN    1024
#define FNAME_LEN   256
#define MIN_DICT_SIZE 1024

#define err(a) {fprintf(stderr, a); exit(EXIT_FAILURE);}

typedef struct buffer_st {
        int pageOffset;
        int chunk_diff;
        int chunk_size;
        int fd;
        char *map;
        char *line;
        char *chunk_end;
} buffer_t;

inline static int    parseLine( const char text[], nod_t * res, int *type );
inline static void   metrace( const char *const fname );
inline static char * buf_init( int fd, buffer_t * bp, size_t size );
inline static size_t buf_readline( char *start, char **end );
       static void   buf_show( buffer_t * bp );
inline static char * buf_rebuf( buffer_t * bp, size_t chunk_size, char isSmall );
inline static void   readSzFile( const char *text, int *sz, char *file, int fileLen );
inline static void   readInt( const char *text, int *res );
inline static void   checkAddress(int type, int ptr, nod_t node, dict_ptr dict) ;

#if 0
int  runTestSuite(void) ;
void test_mgets(void);
void test_readInt(void);
int init_suite1(void){return 0;}
int clean_suite1(void){return 0;}
#endif

#endif
