#include <cstdio>
int main() {
    for(int i=1;i<=0xfff; ++i)
        printf("MEMINFO: new() %#.7x 00 unix/thread.cpp(000){00000, -0000000000}\n",
        i);
    for(int i=1;i<=0xff; ++i)
        printf("MEMINFO: del() %#.7x ../include/common/halloc.h(000){00000, -0000000000}\n",
        i);
        return 0;
}
