#if !defined _MAIL_STORE_
#define _MAIL_STORE_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MailStore.h"

MailStore::MailStore()
{
    int k ;

    arg.p = this ;
    FILE* f = fopen( fname, "r" ) ;
    if( !f ) { printf("Can't open %s\n",fname ) ; exit(1) ; }
    while(!feof(f))
    {
        struct dist_t tmp;
        fscanf( f, "%i %f", &tmp.msg, &tmp.pct );
        md.push_back(tmp);
    }
    fclose(f);

    // calculez pasii pentru md
    for( k=0; k < md.size(); ++k)
    {
        md[k].step = 100.0/md[k].pct ;
        printf("pct[%f]\t-- step[%f]\n", md[k].pct, md[k].step ) ;
    }
}

/**
 * param[in] delay The number of miliseconds to wait after each mail is sent.*/
    void*
MailStore::fillMbox( void* t, const int delay )
{
    struct targ* arg = (struct targ*) t ;
    int i = 0,z,rc=0 ;

    for(;;)
    {
        pthread_mutex_lock( arg->mtx );
        {
            i = arg->mbi ;
            arg->mbi++ ;
        }
        pthread_mutex_unlock( arg->mtx );

        for( int k=0; k < arg->p->md.size() ; k++ )
        {
            if( fmod( i, arg->p->md[k].step) < 1.0 )
            {
                pthread_mutex_lock( arg->mtx );
                if( arg->mbf > arg->end )
                {
                    pthread_mutex_unlock( arg->mtx );
                    printf("all done: exit\n");
                    pthread_exit(0);
                }
                z=arg->mbf++ ;
                pthread_mutex_unlock( arg->mtx ) ;
                rc = printf("Give %i mails to user%i\n", arg->p->md[k].msg, z ) ;

            }
        }
    }
    return 0 ;
}



int
MailStore::init( const char* host, const int port, const int maxMbox, const int threads)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

    int         i ;
    pthread_t   ta[threads] ;
    struct      dist_t tmp ;

    arg.mbi = 0 ;
    arg.mbf = 0 ;
    arg.mtx = &mtx ;
    arg.end = maxMbox ;

    for( i = 0 ; i < threads ; ++i )
    {
        pthread_create( &ta[i], NULL, fillMbox, (void*)&arg) ;
    }
    for( i = 0 ; i < threads ; ++i )
        pthread_join( ta[i], NULL ) ;


    pthread_mutex_destroy(&mtx) ;
    return 0;
}

#endif // _MAIL_STORE_
