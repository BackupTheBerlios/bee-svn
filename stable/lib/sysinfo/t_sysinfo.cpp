#include "SysInfo.h"
#include <iostream>

using namespace std;

int main()
{
    SysInfo r ;
    float one_m, five_m, ften_m;
    int read, sect, mergeRead ;
    cout <<"MemFree:" <<r.memFree() <<endl ;
    cout <<"MemTotal:" <<r.memTotal() <<endl ;
    cout <<"SwapFree:" <<r.swapFree() <<endl ;
    cout <<"SwapTotal:" <<r.swapTotal() <<endl ;
    cout <<"HDD:ReadInfo:" <<endl;
    r.readInfo("hda1", &read, &sect , &mergeRead);
    printf("#Reads:%i Sect:%i mergedRead:%i\n", read, sect, mergeRead);
    r.loadAvg(&one_m, &five_m, &ften_m) ;
    printf("loadAvg: %f %f %f\n", one_m, five_m, ften_m);
    return 0;
}
