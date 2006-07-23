/** @file  Debug.cpp
 *  @brief Usefull in debugging. */
#include "Debug.h"
#include <stdio.h>  //printf, fprintf
#include <stdarg.h> //va_list

//! Decide if debug() will be called. Set by DBG_ENV. @see DBG_ENV
char* useDebug_g=0 ;
FILE* logHandle=0 ;

/** Print debug information in this.log.
 * @param[in] function Name of the calling function.
 * @param[in] line Line from where debug() was called.
 * @param[in] fileName File name from where debug() was called.
 * @param[in] fmt Simmilar to printf. **/
void
_debug( const char* function, int line, const char* fileName, const char* fmt, ...)
{
    if( 0 == useDebug_g ) return ;

    va_list ap ;
    va_start( ap, fmt ) ;
        (void) fprintf( logHandle, "%15s +%d : [%10s] <", fileName, line, function ) ;
        (void) vfprintf( logHandle, fmt, ap ) ;
    va_end( ap ) ;
    (void) fprintf( logHandle, ">\n" ) ;
}//*debug


#if 0
void*
operator new( size_t sz)
{
    void *array[10],*p ;
    size_t size ;
    char **strings ;
    size_t i ;

    size = backtrace (array, 2) ;
    strings = backtrace_symbols (array, size) ;


    p=malloc( sz ) ;
    for (i = 1 ; i < size ; i++)
        fprintf( logHandle, "[+%p] %u\t%s\n", p,sz,strings[i]) ;

    free (strings) ;
    return p ;
}

void
operator delete( void* p)
{
    void *array[10] ;
    size_t size ;
    char **strings ;
    size_t i ;

    size = backtrace (array, 2) ;
    strings = backtrace_symbols (array, size) ;


    for (i = 1 ; i < size ; i++)
        fprintf( logHandle, "[-%p] \t%s\n", p,strings[i]) ;

    free (strings) ;
    free( p ) ;
}



//        const char *file, int line)
//inline void * __cdecl operator new(unsigned int size)
/*inline void * operator new(unsigned int size)
{
    void *ptr = (void *)malloc(size) ;
    printf("alloc\n") ;
    return(ptr) ;
} ;
*/
/*inline void __cdecl operator delete(void *p)
inline void operator delete(void *p)
{
    printf("free\n") ;
    free(p) ;
} ;*/
typedef struct {
    DWORD	address ;
    DWORD	size ;
    char	file[64] ;
    DWORD	line ;
} ALLOC_INFO ;

typedef list<ALLOC_INFO*> AllocList ;

AllocList *allocList ;

void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum)
{
    ALLOC_INFO *info ;

    if(!allocList) {
        allocList = new(AllocList) ;
    }

    info = new(ALLOC_INFO) ;
    info->address = addr ;
    strncpy(info->file, fname, 63) ;
    info->line = lnum ;
    info->size = asize ;
    allocList->insert(allocList->begin(), info) ;
} ;

void RemoveTrack(DWORD addr)
{
    AllocList::iterator i ;

    if(!allocList)
        return ;
    for(i = allocList->begin() ; i != allocList->end() ; i++)
    {
        if((*i)->address == addr)
        {
            allocList->remove((*i)) ;
            break ;
        }
    }
} ;

void DumpUnfreed()
{
    AllocList::iterator i ;
    DWORD totalSize = 0 ;
    char buf[1024] ;

    if(!allocList)
        return ;

    for(i = allocList->begin() ; i != allocList->end() ; i++) {
        sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
                (*i)->file, (*i)->line, (*i)->address, (*i)->size) ;
        OutputDebugString(buf) ;
        totalSize += (*i)->size ;
    }
    sprintf(buf, "-----------------------------------------------------------\n") ;
    OutputDebugString(buf) ;
    sprintf(buf, "Total Unfreed: %d bytes\n", totalSize) ;
    OutputDebugString(buf) ;
} ;


void * operator new (size_t size)
{
    assert (size == sizeof (Link)) ;
    return _freeList.NewLink () ;
}
void operator delete (void * mem)
{
    if (mem)
        _freeList.Recycle (mem) ;
}
void * operator new (size_t size, char const * file, int line)
{
    std::cout << file << ": " << line << std::endl ;
    return ::new char [size] ;
}

int * p = new ("c:\test\main.cpp", 4) int ;
#endif
