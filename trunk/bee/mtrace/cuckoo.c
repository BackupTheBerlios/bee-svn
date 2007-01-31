/*
 * cuckoo.c
 *
 * Dictionary code for cuckoo hashing.
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that due acknoweledgement to the authors are provided and
 * this permission notice appears in all copies of the software.
 * The software is provided "as is". There is no warranty of any kind.
 *
 *
 * Authors:
 *     Rasmus Pagh and Flemming Friche Rodler
 *     BRICS (Basic Research In Computer Science}
 *     Department of Computer Science
 *     University of Aarhus, Denmark
 *     {pagh,ffr}@brics.dk
 * 
 * Date: June 27, 2001.  
 */


#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<time.h>
#include<math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "cuckoo.h"



dict_ptr alloc_dict( int tablesize )
{

        dict_ptr D;

        D = ( dict_ptr ) calloc( 1, sizeof( dict ) );
        D->size = 0;
        D->tablesize = tablesize;
        D->meansize = 5 * ( 2 * tablesize ) / 12;
        D->minsize = ( 2 * tablesize ) / 5;
        D->shift = 32 - ( int )( log( tablesize ) / log( 2 ) + 0.5 );
        D->maxchain = 4 + ( int )( 4 * log( tablesize ) / log( 2 ) + 0.5 );
        if( ( D->T1 =
              ( celltype * ) calloc( tablesize, sizeof( celltype ) ) ) == NULL )
        {
                fprintf( stderr, "Error while allocating mem for T1\n" );
                exit( 0 );
        }
        if( ( D->T2 =
              ( celltype * ) calloc( tablesize, sizeof( celltype ) ) ) == NULL )
        {
                fprintf( stderr, "Error while allocating mem for T2\n" );
                exit( 0 );
        }
        inithashcuckoo( D->a1 );
        inithashcuckoo( D->a2 );

        return ( D );
}


/*------insert taylored to rehash-------------------------------*/
boolean rehash_insert( dict_ptr D, int key, nod_t node )
{
        unsigned long hkey;
        int j;
        celltype x, temp;

        x.key = key;
        x.data = node;
        for( j = 0; j < D->maxchain; j++ )
        {
                hashcuckoo( hkey, D->a1, D->shift, x.key );
                temp = D->T1[hkey];
                D->T1[hkey] = x;
                if( !temp.key )
                        return TRUE;
                x = temp;

                hashcuckoo( hkey, D->a2, D->shift, x.key );
                temp = D->T2[hkey];
                D->T2[hkey] = x;
                if( !temp.key )
                        return TRUE;
                x = temp;
        }

        bzero( D->T1, D->tablesize * sizeof( celltype ) );
        bzero( D->T2, D->tablesize * sizeof( celltype ) );

        inithashcuckoo( D->a1 );
        inithashcuckoo( D->a2 );

        return FALSE;
}                               /* rehash_insert */


/*------rehash--------------------------------------------*/
void rehash( dict_ptr D, int new_size )
{
        dict_ptr D_new;
        int k;

        D_new = alloc_dict( new_size );

        for( k = 0; k < D->tablesize; k++ )
        {
                if( ( D->T1[k].key )
                    &&
                    ( !rehash_insert( D_new, D->T1[k].key, D->T1[k].data ) ) )
                {
                        k = -1;
                        continue;
                }
                if( ( D->T2[k].key )
                    &&
                    ( !rehash_insert( D_new, D->T2[k].key, D->T2[k].data ) ) )
                        k = -1;
        }
        free( D->T1 );
        free( D->T2 );

        D_new->size = D->size;
        *D = *D_new;
        free( D_new );
}                               /* rehash */


/*------construct_dict---------------------------------*/
dict_ptr construct_dict( int min_size )
{
        srand( time( NULL ) );
        return alloc_dict( min_size );
}                               /* construct_dict */


/*------insert-----------------------------------------*/
/*
 * If threshold size is reached, then dump the hash to disk */
boolean insert( dict_ptr D, int key, nod_t node )
{

        unsigned long h1, h2;
        int j;
        celltype x, temp;

        /*
         * If element already in D then replace and return
         */
        hashcuckoo( h1, D->a1, D->shift, key );
        if( D->T1[h1].key == key )
        {
                D->T1[h1].data = node;
                return FALSE;
        }
        hashcuckoo( h2, D->a2, D->shift, key );
        if( D->T2[h2].key == key )
        {
                D->T2[h2].data = node;
                return FALSE;
        }

        /*
         * else insert new element in D 
         */
        if( D->size >= SIZE_THRESHOLD )
        {
            dumpHash( D, "1.db");
            insert( D, key, node );
        }
        x.key = key;
        x.data = node;
        for( j = 0; j < D->maxchain; j++ )
        {
                temp = D->T1[h1];
                D->T1[h1] = x;
                if( !temp.key )
                {
                        D->size++;
                        if( D->tablesize < D->size )
                                rehash( D, 2 * D->tablesize );
                        return TRUE;
                }
                x = temp;
                hashcuckoo( h2, D->a2, D->shift, x.key );

                temp = D->T2[h2];
                D->T2[h2] = x;
                if( !temp.key )
                {
                        D->size++;
                        if( D->tablesize < D->size )
                                rehash( D, 2 * D->tablesize );
                        return TRUE;
                }
                x = temp;
                hashcuckoo( h1, D->a1, D->shift, x.key );
        }

        /*
         * Forced rehash 
         */
        if( D->size < D->meansize )
                rehash( D, D->tablesize );
        else
        {
                rehash( D, 2 * D->tablesize );
        }
        insert( D, x.key, x.data );
        return TRUE;
}                               /* insert */

/*-------lookup--------------------------------------*/
boolean lookup( dict_ptr D, int key )
{
        unsigned long hkey;

        hashcuckoo( hkey, D->a1, D->shift, key );
        if( D->T1[hkey].key == key )
                return TRUE;

        hashcuckoo( hkey, D->a2, D->shift, key );
        return ( D->T2[hkey].key == key );

}                               /* lookup */

/*-------delete---------------------------------------*/
boolean delete( dict_ptr D, int key )
{
        unsigned long hkey;

        hashcuckoo( hkey, D->a1, D->shift, key );
        if( D->T1[hkey].key == key )
        {
                D->T1[hkey].key = 0;
                D->T1[hkey].data.line = 0;
                D->T1[hkey].data.is_new = 0;
                D->T1[hkey].data.fid = 0;
                D->T1[hkey].data.is_newa = 0;
                D->size--;
                if( D->size < D->minsize )
                        rehash( D, D->tablesize / 2 );
                return TRUE;
        } else
        {
                hashcuckoo( hkey, D->a2, D->shift, key );
                if( D->T2[hkey].key == key )
                {
                        D->T2[hkey].key = 0;
                        D->T2[hkey].data.line = 0;
                        D->T2[hkey].data.is_new = 0;
                        D->T2[hkey].data.fid = 0;
                        D->T2[hkey].data.is_newa = 0;
                        D->size--;
                        if( D->size < D->minsize )
                                rehash( D, D->tablesize / 2 );
                        return TRUE;
                }
        }
        return FALSE;
}                               /* delete */


int dumpHash( dict_ptr D, const char* fname )
{
    int fd=-1,i;
    fd = open( fname, O_RDWR|O_CREAT | O_TRUNC, S_IRWXU);
    int tb_size=0;
    if( fd ==-1 )
    {
        fprintf(stderr, "cannot open %s : %s\n",
                fname, strerror(errno) ) ;
        exit(EXIT_FAILURE);
    }
    tb_size = D->size*sizeof(celltype);
    printf("dumping sz=%d szT1=%d\n", D->size, tb_size);
    for(i=0;i<10; i++) {
        printf("t1[i]=%d-t2[i]=%d\n", D->T1[i], D->T2[i]);
    }
    write(fd, (void*)D, sizeof(int)*6);
    write(fd, (void*)(D->T1), tb_size );
    write(fd, (void*)(D->T2), tb_size );
    close(fd);
    D->size =0;
}

dict_ptr loadHash( const char* fname )
{
    int fd=-1, tb_size=0;
    struct cell *T;
    dict_ptr D = ( dict_ptr ) calloc( 1, sizeof( dict ) );


    fd = open(fname, O_RDWR );
    read(fd, D ,sizeof(int)*6);
    printf("-------Size:%d-----------\n", D->size);
    T = calloc(D->size,sizeof(celltype));
    tb_size = D->size*sizeof(celltype);
    D->T1 = ( celltype * ) calloc( D->size, sizeof( celltype ) ) ;
    D->T2 = ( celltype * ) calloc( D->size, sizeof( celltype ) ) ;

    read(fd, T,tb_size );
    memcpy( D->T1, T, tb_size);

    read(fd, T,tb_size );
    memcpy( D->T2, T, tb_size);

    close(fd);
    return D;
}

/*-------size-------------------------------------------*/
int size( dict_ptr D )
{
        return ( D->size );
}                               /* size */


/*-------clear------------------------------------------*/
void clear( dict_ptr D, int min_size )
{
        dict_ptr D_new;

        D_new = construct_dict( min_size );
        free( D->T1 );
        free( D->T2 );
        *D = *D_new;
}                               /* clear */

/*--------destruct_dict-----------------------------------*/
dict_ptr destruct_dict( dict_ptr D )
{
        int i;
        for( i = 0; i < D->tablesize; ++i )
        {
                // delete( D->T1[i] );
                // delete( D->T2[i] );
        }
        free( D->T1 );
        free( D->T2 );
        free( D );
        return ( NULL );
}                               /* destruct_dict */
