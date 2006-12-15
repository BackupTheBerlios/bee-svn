/*
 *  indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c
 *  gcc -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops mmtrace.c -o mtrace
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


/*----------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
        if( argc < 3 )
                err( "Usage: ./memtrace <debug.log> <run_tests>\n" );

        sprintf( buf, "cat /proc/%d/status >>stats", getpid(  ) );

        if( argv[2][0] != 'y' )
        {
                runTestSuite(  );
                return 0;
        }

        memtrace( argv[1] );
        system( buf );
        return 0;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
inline static char *
buf_init( int fd, buffer_t * bp, size_t size )
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
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
inline static char *
buf_rebuf( buffer_t * bp, size_t chunk_size, char isSmall )
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

        if(isSmall)
            bp->chunk_end = bp->map + chunk_size;
        else
            bp->chunk_end = memrchr( bp->map, '\n', chunk_size );

        bp->pageOffset += chunk_size - 4096;
        bp->chunk_diff =
            ( char * )bp->chunk_end - ( bp->map + ( chunk_size - 4096 ) ) ;

        return bp->map;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
static void
memtrace( const char *const fname )
{
        char *p = NULL, *end = 0;
        int fd = -1;
        struct stat statbuf;
        dict_ptr dict;
        buffer_t bp = {.b_line = 0,.b_map = 0,.b_pageOffset = 0,.b_fileOffset =
                    0,.b_chars = 0
        };

        if( ( fd = open( fname, O_RDWR ) ) < 0 )
                err( "Unable to open debug file\n" );

        dict = construct_dict( MIN_DICT_SIZE );
        fstat( fd, &statbuf );

        buf_init( fd, &bp, size );

        if( statbuf.st_size >= 10240 )
        while( bp.pageOffset < ( size - size % bp.chunk_size - ( size / bp.chunk_size ) * 4096 ) )
        {
                buf_rebuf( &bp, bp.chunk_size, 0 );
                while( bp.line < bp.chunk_end )
                {
                        tmp = buf_readline( bp.line, &end );
                        ++linesRead;
                        printf( "%s\n", bp.line );
                        if( bp.line[0] != 'M' && bp.line[6] != 'O' )
                            continue;
                        p = bp.line + 9;      /* Advance over 'MEMINFO: ' */

                        ptr = parseLine( p, &nod, &type );
                        checkAddress( type, ptr, nod, dict );
                        bp.line = end;
                }
        }

        /* st_size < 1Mb */
        left_to_read = statbuf.st_size - bp->pageOffset;
        buf_rebuf(&bp, left_to_read,1 );
        while( bp.line != bp.chunk_end )
        {
                tmp = buf_readline( bp.line, &end );
                ++linesRead;
                printf( "%s\n", bp.line );
                if( bp.line[0] != 'M' && bp.line[6] != 'O' )
                    continue;
                p = bp.line + 9;

                ptr = parseLine( p, &nod, &type );
                checkAddress( type, ptr, nod, dict );
                bp.line = end;
        }
        close( fd );
        destruct_dict( dict );
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
inline static
void readInt( const char *text, int *sz )
{
        for( *sz = 0;
             *text != '\0' && *text != ' ' &&
             *text >= '0' && *text <= '9'; ++text )
                *sz = ( *sz ) * 10 + *text - '0';
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
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

        for( i = 0; *text != '\0'; ++i, ++text )
            ;

        /* 0.1 % cache misses */
        memcpy( file, text + 1, i );
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
inline void
checkAddress( int type, int ptr, nod_t nod, dict_ptr dict )
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
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*----------------------------------------------------------------------------*/
inline static int
parseLine( const char *const text, nod_t * res, int *type )
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
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
