#include "SysInfo.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    SysInfo r ;
    float one_m, five_m, ften_m;
    unsigned int read, sect, mergeRead ;

    if(argc < 2 ) { printf("Tell me in seconds, how long will I run !\n"); return 1 ;}

    int seconds=atoi(argv[1]);
    if(seconds <= 0) { printf("Seconds means 1..3600+\n"); return 1; }
    
    for(int i=0; i<seconds; ++i)
    {
        cout <<"MemFree:" <<r.memFree() <<endl ;
        cout <<"MemTotal:" <<r.memTotal() <<endl ;
        cout <<"SwapFree:" <<r.swapFree() <<endl ;
        cout <<"SwapTotal:" <<r.swapTotal() <<endl ;
        cout <<"HDD:ReadInfo:" <<endl;
        r.readInfo("hda1", &read, &sect , &mergeRead);
        printf("#Reads:%lu Sect:%lu mergedRead:%lu\n", read, sect, mergeRead);
        r.loadAvg(&one_m, &five_m, &ften_m) ;
        printf("loadAvg: %f %f %f\n", one_m, five_m, ften_m);
        sleep(1);
    }
    return 0;
}
