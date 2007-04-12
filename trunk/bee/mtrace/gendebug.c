#include <stdio.h>
#include <stdlib.h>
int main( int argc, char* argv[]) {
    int i=0, top=0xf;

    if(argc == 2 ) top = atoi(argv[1]);

    for(i=1;i<=top; ++i)
        printf("MEMINFO: new() %#.7x 00 unix/thread.cpp(000){00000, -0000000000}\n",
        i);
    for(i=1;i<=top; ++i)
        printf("MEMINFO: del() %#.7x ../include/common/halloc.h(000){00000, -0000000000}\n",
        i);
        return 0;
}
