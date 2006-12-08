/*
 * gcc -mpentium -O2 -mmmx -minline-all-stringops mtrace.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/*
 * MEMINFO(("new() " PTR_FORMAT " %d %s(%d){%d, %d}", p, sz, dd.file, dd.line,
 * getpid(), Thread::getCurrentThreadId()));
 */

/*
 * 4 bytes for 1 new() fits in one %movsw 
 */
typedef struct {
         unsigned short int line:15;    /* Line where was allocated */
         unsigned short int is_new:1;   /* new() operator */
         unsigned short int fid:15;     /* file id */
         unsigned short int is_newa:1;  /* new[] operator */
} nod_t;

#define LINE_LENGTH 2048

#define IS_NEW  1
#define IS_NEWA 2

#define IS_DEL  3
#define IS_DELA 4

// #define DEBUG 1

#ifdef DEBUG
#define dprintf(a) printf a
#else
#define dprintf(a) ;
#endif

inline int parseLine( const char text[], nod_t * res, int *type );

int
main( int argc, char *argv[] )
{

         if( argc < 2 )
         {
                  fprintf( stderr, "Usage: mleak debug.log\n" );
                  exit( EXIT_FAILURE );
         }
         mtrace( argv[1] );
         return 0;
}

#if 0
inline int
mmtrace( const char *const fname )
{
         nod_t nod;
         char line[LINE_LENGTH] = { 0 }, *p = NULL;
         int no_bits, fd = -1, type = 0;
         char *map = 0;
         struct stat statbuf;
         FILE *f = 0;

         fd = open( fname, O_RDWR );

         if( fd < 0 )
         {
                  printf( "Unable to open '%s'\n", argv[1] );
                  exit( EXIT_FAILURE );
         }

         fstat( fd, &statbuf );
         map = mmap( 0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                     fd, 0 );
         if( map == MAP_FAILED )
         {
                  printf( "mmap error for input\n" );
                  exit( EXIT_FAILURE );
         }
         close( fd );
}
#endif


inline int
mtrace( const char *const fname )
{
         nod_t nod;
         char line[LINE_LENGTH] = { 0 }, *p = NULL;
         int no_bits, fd = -1, type = 0;
         struct stat statbuf;
         FILE *f = 0;

         f = fopen( fname, "r" );
         if( !f )
         {
                  printf( "err open\n" );
                  exit( -1 );
         }
         while( !feof( f ) )
         {
                  if( !fgets( line, LINE_LENGTH - 1, f ) )
                  {
                           dprintf( ( "Error or EOF encountered while reading file\n" ) );
                           exit( EXIT_FAILURE );
                  }

                  if( line[0] != 'M' && line[6] != 'O' )
                           continue;

                  p = line + 9;

                  parseLine( p, &nod, &type );

                  switch ( type )
                  {
                  case IS_NEW:
                           dprintf( ( "---new()--\n" ) );
                           /*
                            * Insert in Hash 
                            */
                           break;

                  case IS_NEWA:
                           dprintf( ( "---new[]--\n" ) );
                           /*
                            * Insert in Hash 
                            */
                           break;

                  case IS_DEL:
                           dprintf( ( "---delete()--\n" ) );
                           /*
                            * Find in Hash if( found ) if( !typesMatch )
                            * //printf("Mismatch operator"); HashDel( new ); else
                            * //printf("double free\n"); 
                            */
                           break;

                  case IS_DELA:
                           dprintf( ( "---delete[]--\n" ) );
                           /*
                            * Find in Hash 
                            */
                           break;
                  default:
                           dprintf( ( "Unknown operator\n" ) );
                           break;
                  }
         }

         fclose( f );
         /*
          * dump memleaks 
          */
         return 0;
}


#define FILE_LEN 128
inline int
parseLine( const char *const text, nod_t * res, int *type )
{
         char op[8];            /* operator ( new or delete ) */
         char file[FILE_LEN];   /* file */
         int ptr;               /* pointer */
         char *p;               /* used to find the line Number */
         short int sz = 0, line = 0;

         sscanf( text, "%s %x", op, &ptr );

         if( op[0] == 'n' && op[4] == ')' )
         {
                  dprintf( ( "+++new()++\n" ) );
                  res->is_new = 1;
                  *type = IS_NEW;
                  sscanf( text + 16, "%*d %s", &sz, file );
         } else if( op[0] == 'n' && op[4] == ']' )
         {
                  dprintf( ( "+++new[]++\n" ) );
                  res->is_newa = 1;
                  *type = IS_NEWA;
                  sscanf( text + 16, "%*d %s", &sz, file );
         } else if( op[0] == 'd' && op[4] == ')' )
         {
                  dprintf( ( "+++delete()++\n" ) );
                  *type = IS_DEL;
                  sscanf( text + 16, "%s", file );
         } else if( op[0] == 'd' && op[4] == ']' )
         {
                  dprintf( ( "+++delete[]++\n" ) );
                  *type = IS_DELA;
                  sscanf( text + 16, "%s", file );
         }

         p = memchr( file, '(', FILE_LEN );
         if( !p )
                  return 0;
         *p = 0;
         sscanf( p + 1, "%d", &line );
         dprintf( ( "OP=%s HEX=%x SZ=%d FILE=%s LINE=%d\n", op, ptr, sz, file,
                    line ) );

         res->line = line;
         return ptr;
}
