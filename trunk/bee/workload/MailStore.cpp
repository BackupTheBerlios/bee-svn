#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MailStore.h"

MailStore::MailStore()
{
    int k ;
    char fn[] = "md.txt" ;

    arg.p = this ;
    FILE* f = fopen( fn, "r" ) ;
    if( !f ) { printf("cant open %s\n",fn ) ; exit(1) ; }
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
        printf("pct=%f--step=%f\n", md[k].pct, md[k].step ) ;
    }
}

/*
 * Ar trebui sa fie multi-thread */
    void*
MailStore::fillMbox( void* t )
{
    struct targ* arg = (struct targ*) t ;
    int i = 0,rc=0 ;
    for(;;) {
        pthread_mutex_lock( arg->mtx );
        if( arg->mbi >= arg->end )
        {
            pthread_mutex_unlock( arg->mtx );
            break ;
        }
        arg->mbi++ ;
        i = arg->mbi ;
//        printf("Current mboxIdx%i\n",i);
        pthread_mutex_unlock( arg->mtx );
        //rc = sendFile() ;
        int k ;
        for( k=0; k < arg->p->md.size() ; k++ )
        {
            if( fmod( i,arg->p->md[k].step) < 1.0 )
            {
                rc = printf("%i mails for user%i\tStep=%f\n",
                    arg->p->md[k].msg, i,arg->p->md[k].step ) ;
//                break ;
                if( !rc ){
                    // If it failed, let other thread try send the mail
                    pthread_mutex_lock( arg->mtx );
                    arg->mbi-- ;
                    pthread_mutex_unlock( arg->mtx ) ;
                }
            }
        }// sendFile
    }
//    printf("all done: exit\n");
    pthread_exit(0);
    return 0 ;
}

int
MailStore::init( const char* host, const int port, const int maxMbox, const int threads)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

    int         mboxIdx=0, i ;
    pthread_t   ta[threads] ;
    struct      dist_t tmp ;

    arg.mbi = mboxIdx ;
    arg.mtx = &mtx ;
    arg.end = maxMbox ;

    for( i = 0 ; i < threads ; ++i )
    {
        pthread_create( &ta[i], NULL, fillMbox, (void*)&arg) ;
    }
    for( i = 0 ; i < threads ; ++i ) 
        pthread_join( ta[i], NULL ) ;


    pthread_mutex_destroy(&mtx);  
    return 0;
}

int main() {
    MailStore ms ;
    ms.init("localhost", 25, 100,1 ) ;
    return 0;
}
