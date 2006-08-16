#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/dir.h>


/* obtain max num cpus from kernel multiply by 2 and add 2 */
#define MAX_NR_CPUS  ((NR_CPUS*2)+2)


/*Resource statistics */
class SysInfo {
    public:
        SysInfo() ;
       ~SysInfo() ;
       
       float memFree( ) ;
       float memTotal( ) ;
       float swapFree( ) ;
       float swapTotal( ) ;
       int loadAvg( float*, float*, float*) ;
       // param[in]  iface
       // param[out] bytes
       // param[out] packets
       int netRx( const char* iface, long int* bytes, long int* pkts ) ;
       int netTx( const char* iface, long int* bytes, long int* pkts ) ;
       // param[in] device

       int readInfo( const char* device, unsigned int* reads, unsigned int* sectors, unsigned int* mergedReads );
       int writeInfo( const char* device, unsigned int* writes, unsigned int* sectors );
       int reads( const char* device ) ;
       int writes( const char* device ) ;
} ;
