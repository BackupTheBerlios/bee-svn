#include <stdio.h>
#include "cuckoo.h"

int main() {
        int i=0;
        nod_t A={.line=12,.fid=1,.is_new=1};
        dict_ptr dict = construct_dict( 5 );
        dict_ptr ldict = construct_dict( 5 );

        for(i=0;i<11;i++) {
                insert( dict, i, A);
        }
        /*Insert one more, so the table gets dumped */
        insert( dict, i, A);

        printf("Printing in-memory dict\n");
        for(i=0;i<dict->size;i++)
                printf( "t1[%i]=%3d || t2[%i]=%3d\n", i,dict->T1[i].key, i,dict->T2[i].key);

        printf("Loading...\n");
        ldict = loadHash("0.db");

        printf("Printing loaded dict\n");
        for(i=0;i<ldict->size;i++)
                printf( "t1[%i]=%3d || t2[%i]=%3d\n", i,ldict->T1[i].key, i,ldict->T2[i].key);
//        destruct_dict(dict);
        return 0;
}
