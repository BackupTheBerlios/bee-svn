
#include "SysInfo.h"
#include <errno.h>
/*Resource statistics */

SysInfo::SysInfo()
{
};

SysInfo::~SysInfo()
{
}

float
SysInfo::memFree()
{
    int     fh ;
    char    buf[1024],*pc ;
    int     br=0 ;  // bytes read
    float   mf ;    // memory free
    char    fname[] ="/proc/meminfo" ;
    fh = open( fname ,O_RDONLY) ;
    br = read( fh, buf, 1024) ;
    close( fh ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname ) ;
        return 0 ;
    }
    buf[br] = '\0' ;
    pc = strstr( buf, "MemFree:" ) ;
    if( !pc )
    {
        printf("Cant find memfree record\n");
        return 0 ;
    }
    sscanf( pc, "MemFree: %f", &mf ) ;
    return mf ;
}

float
SysInfo::memTotal()
{
    int     fh ;
    char    buf[1024],*pc ;
    int     br=0 ;  // bytes read
    float   mf ;    // memory free
    char    fname[] ="/proc/meminfo" ;
    fh = open( fname ,O_RDONLY) ;
    br = read( fh, buf, 1024) ;
    close( fh ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname ) ;
        return 0 ;
    }
    buf[br] = '\0' ;
    pc = strstr( buf, "MemTotal:" ) ;
    if( !pc )
    {
        printf("Cant find memfree record\n");
        return 0 ;
    }
    sscanf( pc, "MemTotal: %f", &mf ) ;
    return mf ;
}

float
SysInfo::swapFree()
{
    int     fh ;
    char    buf[1024],*pc ;
    int     br=0 ;  // bytes read
    float   mf ;    // memory free
    char    fname[] ="/proc/meminfo" ;
    fh = open( fname ,O_RDONLY) ;
    br = read( fh, buf, 1024) ;
    close( fh ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading meminfo\n") ;
        return 0 ;
    }
    buf[br] = '\0' ;
    pc = strstr( buf, "SwapFree:" ) ;
    if( !pc )
    {
        printf("Cant find memfree record\n");
        return 0 ;
    }
    sscanf( pc, "SwapFree: %f", &mf ) ;
    return mf ;
}

float
SysInfo::swapTotal()
{
    int     fh ;
    char    buf[1024],*pc ;
    int     br=0 ;  // bytes read
    float   mf ;    // memory free
    char    fname[] ="/proc/meminfo" ;
    fh = open( fname ,O_RDONLY) ;
    br = read( fh, buf, 1024) ;
    close( fh ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname ) ;
        return 0 ;
    }
    buf[br] = '\0' ;
    pc = strstr( buf, "SwapTotal:" ) ;
    if( !pc )
    {
        printf("Cant find memfree record\n");
        return 0 ;
    }
    sscanf( pc, "SwapTotal: %f", &mf ) ;
    return mf ;
}



/*
 * An idle computer has a load number of 0.
 * Each process that is using CPU
 * OR waiting for CPU
 * OR is in uninterruptible sleep (usually waiting for disk activity) adds 1
 * to the load number.
 *
 * The load average is calculated as the
 * exponentially damped moving average of the load number.
 * The three values of load average refer to the past
 * _one_, _five_, and _fifteen_ Minutes of system operation.
 */
int
SysInfo::loadAvg(float* one_m, float* five_m, float* ften_m ) {
    int     fh ;
    char    buf[64];
    char    fname[] ="/proc/loadavg" ;
    
    fh = open( fname ,O_RDONLY) ;
    int br = read( fh, buf, 63) ;
    close( fh ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %sn", fname ) ;
        return 0 ;
    }
    buf[br] = '\0' ;
    int pc = sscanf( buf, "%f %f %f", one_m, five_m, ften_m ) ;
    if( !pc )
    {
        printf("Cant find memfree record\n");
        return 0 ;
    }
    return 1 ;
}


int
SysInfo::netRx( const char* iface, long int* bytes, long int* pkts )
{
    int     fh ;
    char    fname[] ="/proc/net/dev" ;
    int     bufsz = 8192 ;
    char    buf[bufsz];
    char*   loc =0;
    
    fh = open( fname, O_RDONLY ) ;
    int br = read( fh, buf, bufsz ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname ) ;
        return 0 ;
    }
    loc = strstr( buf, iface ) ;
    if( !loc )
    {
        printf("Can't find interface %s\n", iface ) ;
        return 0 ;
    }
    br = sscanf( loc, "%*s: %i %i", bytes, pkts ) ;
    if( !br )
    {
        printf( "Can't get info from %s", fname ) ;
        return 0 ;
    }
    return 1 ;
}

int
SysInfo::netTx( const char* iface, long int* bytes, long int* pkts )
{
    int     fh ;
    char    fname[] ="/proc/net/dev" ;
    int     bufsz = 8192 ;
    char    buf[bufsz];
    char*   loc =0;
    
    fh = open( fname, O_RDONLY ) ;
    int br = read( fh, buf, bufsz ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname ) ;
        return 0 ;
    }
    /* Start parsing */
    loc = strstr( buf, iface ) ;
    if( !loc )
    {
        printf("Can't find interface %s\n", iface ) ;
        return 0 ;
    }
    br = sscanf( loc, "%*s: %*i %*i %*i %*i %*i %*i %*i %*i %i %i", bytes, pkts ) ;
    if( !br )
    {
        printf( "Can't get info from %s", fname ) ;
        return 0 ;
    }
    return 1 ;
}





int
SysInfo::readInfo( const char* device, unsigned int* reads, unsigned int* sectors, unsigned int* mergedReads )
{
    int fh,br=0 ;
    int     bufsz = 8192 ;
    char    buf[bufsz];
    char fname[] = "/proc/diskstats" ;
   
    fh = open( fname, O_RDONLY ) ;
    br = read( fh, buf, bufsz ) ;
    if( !br || sizeof(buf) == br )
    {
        printf("trouble reading %s\n", fname) ;
        return 0 ;
    }

    /* Start parsing */
    char* loc = strstr( buf, device ) ;
    if( !loc )
    {
        printf("Can't find device %s\n", device ) ;
        return 0 ;
    }
    br = sscanf( loc, "%*s %u %u %u" , &reads, &sectors, &mergedReads ) ;
    if( !br )
    {
        printf( "Can't get info from %s\n", fname ) ;
        return 0 ;
    }
    return 1 ;
}

int
SysInfo::writeInfo( const char* device, unsigned int* writes, unsigned int* sectors )
{
}
