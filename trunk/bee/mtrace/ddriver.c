#include<stdio.h>
#include<stdlib.h>
#include "cuckoo.h"

/*----------------------------------------------*/
/* Dictionary driver based on the DIMACS driver */
/* by Catherine C. McGeoch                      */
/*                                              */
/* Modified by Rasmus Pagh and Flemming Rodler  */
/* June 2001.                                   */
/*----------------------------------------------*/
#define DETAIL 0

/* Driver reads commands, calls routines, reports */
int main( int argc, char **argv )
{
        dict_ptr dictionary;
        int key, tmp;
        boolean found;
        char index;
        int i, min_size;


        min_size = 1024;
        /*
           switch(argc) {
           case 2: min_size = atoi(argv[1]);
           case 1:
           break;
           default:
           fprintf(stderr,"Usage: %s [min size]\n",argv[0]);
           exit(1);
           }
         */
        /* Perform dictionary operations */
        dictionary = construct_dict( min_size );
        if( DETAIL > 0 ) {
                printf( "dch" );
                fflush( stdout );
        }
        printf( "\n" );
        for( i = 0; i < 1200000; ++i ) {
                found = insert( dictionary, i );
                if( DETAIL > 0 ) {
                        if( found )
                                printf( "1" );
                        else
                                printf( "0" );
                }
                /*retrieve */
                tmp = size( dictionary );
                if( DETAIL >= 1 )
                        printf( "[%d]", tmp );
        }
        printf( "\n" );
#if 0
        while( fread( &index, 1, 1, stdin ) == 1 ) {
                switch ( index ) {
                case '1':{     /* Reset counters */
                                break;
                        }
                case '3':{     /* Insert element */
                                fread( &key, sizeof( int ), 1, stdin );
                                break;
                        }
                case '4':{     /* Lookup element */
                                fread( &key, sizeof( int ), 1, stdin );
                                found = lookup( dictionary, key );

                                if( DETAIL >= 1 ) {
                                        if( found )
                                                printf( "1" );
                                        else
                                                printf( "0" );
                                }
                                break;
                        }
                case '5':{     /* Delete element */
                                fread( &key, sizeof( int ), 1, stdin );
                                found = delete( dictionary, key );
                                if( DETAIL >= 1 ) {
                                        if( found )
                                                printf( "1" );
                                        else
                                                printf( "0" );
                                }
                                break;
                        }
                case '9':{     /* Retrieve size */
                                break;
                        }
                case '0':{     /* Clear dictionary */
                                clear( dictionary, min_size );
                                if( DETAIL > 0 )
                                        printf( "clr" );
                                break;
                        }
                }
        }
#endif
        destruct_dict( dictionary );

        return 0;
}
