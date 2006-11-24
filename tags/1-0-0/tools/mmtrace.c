/*
 * gcc -march=i686 -O3 -mmmx -minline-all-stringops mmtrace.c -o mtrace */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/* MEMINFO(("new() " PTR_FORMAT " %d %s(%d){%d, %d}", p, sz, dd.file, dd.line, getpid(), Thread::getCurrentThreadId()));*/

/* 4 bytes for 1 new() fits in one %movsw */
typedef struct {
        unsigned short int line : 15;   /* Line where was allocated */
        unsigned short int is_new : 1;  /* new() operator */
        unsigned short int fid :15 ;    /* file id */
        unsigned short int is_newa : 1; /* new[] operator */
} nod_t ;

#define LINE_LENGTH 2048

#define IS_NEW  1
#define IS_NEWA 2

#define IS_DEL  3
#define IS_DELA 4

//#define DEBUG 1

#ifdef DEBUG
        #define dprintf(a) printf a 
#else
        #define dprintf(a) ;
#endif

inline int
parseLine(const char text[], nod_t* res, int * type) ;

inline void
mtrace(const char* const fname );


/* This gets called verry often
 */

inline void
mgets(char* map, char** end) {
    for( ; *map != '\n'; ++map) 
        ;

    *end = map ;
    *(*end) = '\0';
}

int main( int argc, char* argv[]){

        if(argc < 2 ) {
                fprintf( stderr, "Usage: mleak debug.log\n");
                exit(EXIT_FAILURE);
        }
        mtrace(argv[1]);
        return 0;
}


inline void
mtrace(const char* const fname )
{
        nod_t   nod ;
        char    *line, *p=NULL;
        int     no_bits, fd = -1, type=0 ;
        struct  stat statbuf;
        size_t  i;
        char* map, * end;
        fd = open( fname , O_RDWR);

        if( fd < 0 ) {
                printf("Unable to open '%s'\n", fname);
                exit(EXIT_FAILURE);
        }

        fstat(fd, &statbuf);
        map = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0) ;
        if( map == MAP_FAILED ) {
                printf("mmap error for input\n");
                exit(EXIT_FAILURE);
        }

        line = map;
        for( end=map;
            map != line+ statbuf.st_size;
            *end = '\n',
            map = end+1 ) {

            mgets( map, &end );

            //line = map ;
            if( map[0] != 'M' && map[6] != 'O' ) {
                continue;
            }

            p = map + 9 ;

            parseLine( p, &nod, &type );
            switch(type)
            {
                case IS_NEW:
                    dprintf(("---new()--\n"));
                    /* Insert in Hash */
                    break;

                case IS_NEWA:
                    dprintf(("---new[]--\n"));
                    /* Insert in Hash */
                    break;

                case IS_DEL:
                    dprintf(("---delete()--\n"));
                    /* Find in Hash
                     * if( found )
                     *      if( !typesMatch )
                     *              //printf("Mismatch operator");
                     *      HashDel( new );
                     * else
                     *      //printf("double free\n");
                     */
                    break;

                case IS_DELA:
                    dprintf(("---delete[]--\n"));
                    /* Find in Hash */
                    break;
                default:
                    dprintf(("Unknown operator\n"));
                    break;
            }
            /**/
        }
        close(fd);
}

inline int
parseLine(const char*const text, nod_t * res, int * type) {
        char op[8];         /* operator ( new or delete )*/
        char file[128] ;    /* file */
        int  ptr  ;         /* pointer */
        char* p;            /* used to find the line Number */
        short int sz =0, line=0;

        sscanf( text, "%s %x", op, &ptr );
        if ( op[0] == 'n' && op[4] == ')' ) {
                dprintf(("+++new()++\n"));
                res->is_new = 1;
                *type = IS_NEW ;
                sscanf( text+16, "%*d %s", &sz, file );
        }else if ( op[0] == 'n' && op[4] == ']' ) {
                dprintf(("+++new[]++\n"));
                res->is_newa = 1;
                *type = IS_NEWA ;
                sscanf( text+16, "%*d %s", &sz, file );
        } else if ( op[0] == 'd' && op[4] == ')' ) {
                dprintf(("+++delete()++\n"));
                *type = IS_DEL ;
                sscanf( text+16, "%s", file );
        } else if ( op[0] == 'd' && op[4] == ']' ) {
                dprintf(("+++delete[]++\n"));
                *type = IS_DELA ;
                sscanf( text+16, "%s", file );
        }

        p = strchr( file, '(');
        if(!p) return 0;
        *p = 0;
        sscanf( p+1, "%d", &line );
        dprintf(("OP=%s HEX=%x SZ=%d FILE=%s LINE=%d\n", op, ptr, sz, file, line ));

        res->line = line ;
        return ptr ;
}

