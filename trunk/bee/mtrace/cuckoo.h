/*
 * cuckoo.h
 *
 * Header file for cuckoo hashing.
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
 * Date: June 27, 2001.  
*/

#ifndef CUCKOO_H
#define CUCKOO_H 



/* Keeps meta-data one new().
 * Uses only 4 bytes to fit in one %movsw */
/* Test if BITWISE operation is faster
 * than bitfields */
typedef struct {
        unsigned short int line:15;     /* Line where was allocated */
        unsigned short int is_new:1;    /* new() operator */
        unsigned short int fid:15;      /* file id */
        unsigned short int is_newa:1;   /* new[] operator */
} nod_t;

typedef struct cell {       /* hash table cell type */ 
  int key;
  nod_t data ;
} celltype;

typedef struct {            /* dictionary type */ 
  int size;                 /* current size */
  int shift;                /* value used for hash function */
  int tablesize;            /* size of hash tables */
  int minsize,meansize;     /* rehash trigger sizes */
  int maxchain;             /* max. iterations in insert */
  struct cell *T1;          /* point to hash table 1*/
  struct cell *T2;          /* point to hash table 2*/
  int a1[3];                /* hash function 1 */
  int a2[3];                /* hash function 2 */
} dict;

typedef dict* dict_ptr;

#define TRUE 1
#define FALSE 0

typedef int boolean;

extern dict_ptr    construct_dict(int min_size); 
extern boolean     insert       (dict_ptr D, int key, nod_t nod);
extern boolean     lookup       (dict_ptr D, int key); 
extern boolean     delete       (dict_ptr D, int key); 
extern int         keyval       (dict_ptr D, int key);
extern int         size         (dict_ptr D); 
extern void        clear        (dict_ptr D, int min_size); 
extern dict_ptr    destruct_dict(dict_ptr D); 
extern int dumpHash( dict_ptr D, const char* fname );
extern dict_ptr loadHash( const char* fname );

/* The below hash function was found to work well in practice */
/* There is no proof that this is always the case, and there  */
/* may be a better choice of function.                        */

#define inithashcuckoo(a) \
{\
  a[0] = ((int)rand() << 1) + 1;\
  a[1] = ((int)rand() << 1) + 1;\
  a[2] = ((int)rand() << 1) + 1;\
}

#define hashcuckoo(h,a,shift,key)\
{\
  h = (a[0]*key) ^ (a[1]*key) ^ (a[2]*key);\
  h = (unsigned int)h >> shift;\
}


#endif 
