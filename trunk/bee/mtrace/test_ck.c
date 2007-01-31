#include <stdio.h>
#include "cuckoo.h"

int main() {
        int i=0;
        nod_t A={.line=12,.fid=1,.is_new=1};
        dict_ptr dict = construct_dict( 5 );
        dict_ptr ldict = construct_dict( 5 );
        for(i=0;i<12;i++)
                insert( dict, i, A);
        ldict = loadHash("1.db");
        for(i=0;i<ldict->size;i++)
                printf( "t1[i]=%d t2[i]=%d\n", ldict->T1[i], ldict->T2[i]);
//        destruct_dict(dict);
        return 0;
}
