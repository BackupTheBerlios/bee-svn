#include "SysInfo.h"
#include <iostream>

using namespace std;

int main()
{
    SysInfo r ;
    float one_m, five_m, ften_m;
    unsigned int read, sect, mergeRead ;
    for(int i=0; i<10; ++i)
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
