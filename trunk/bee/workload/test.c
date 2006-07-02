#include <stdio.h>
#include <stdlib.h>
#include <math.h>
struct dist_t {
    int   msg;
    float probl ;
    float step ;
} ;
/* Message distribution over mailbox */
struct dist_t md[] ={
    {0  , 26.50, 0},
    {1  , 16.11, 0},
    {2  , 12.25, 0},
    {3  , 9.61 , 0},
    {4  , 7.56 , 0},
    {5  , 5.95 , 0},
    {6  , 4.68 , 0},
    {7  , 3.69 , 0},
    {8  , 2.90 , 0},
    {10 , 1.80 , 0},
    {11 , 1.42 , 0},
    {12 , 1.12 , 0},
    {13 , 0.88 , 0},
    {14 , 0.69 , 0},
    {15 , 0.54 , 0},
    {16 , 0.43 , 0},
    {17 , 0.34 , 0},
    {18 , 0.27 , 0},
    {20 , 0.16 , 0},
    {21 , 0.13 , 0},
    {22 , 0.10 , 0},
    {23 , 0.08 , 0},
    {24 , 0.06 , 0},
    {25 , 0.05 , 0},
    {26 , 0.04 , 0},
    {27 , 0.03 , 0},
    {28 , 0.02 , 0},
};
/*
 * Functia ia ca parametru o tabela ca md,
 * si apeleaza o functie callback ?*/
int distribute( struct dist_t md[], int size ) 
{
    int k ;

    // calculez pasii.
    for( k=0; k < size; ++k)
    {
        md[k].step = 100.0/md[k].probl ;
        printf("probl=%f--step=%f\n", md[k].probl, md[k].step ) ;
    }
    return 0 ;
}

int
prepare_mail( int i, struct dist_t md[], int size )
{
    int k ;
    for( k=0; k < 29; k++ )
    {
        if( fmod(i,md[k].step) < 1.0 )
        {
            // aici o sa umpluu mbox(user_i) cu md[k].msg mesaje
            printf("Sending %i mails to user%i\n",md[k].msg, i ) ;
        }
    }
    return 0 ;
}


int main() {
    int k, mb=10000, i=0 ;
    float x = 0.0;

    distribute(md, 29 ) ;

    // Rulez
    for( i=0; i < mb; ++i)
    {
        prepare_mail( i, md, 29 ) ;
    }
    return 0;
}
