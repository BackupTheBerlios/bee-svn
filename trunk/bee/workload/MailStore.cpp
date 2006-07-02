#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <vector>
using namespace std ;



class MailStore {
    public:
        MailStore() ;
        ~MailStore() {};
        // structura folosita de fillMbox
        struct targ {
            int mbi ;               // mailbox index
            pthread_mutex_t* mtx ;  // mutex pt index
            MailStore* p ;
            int end ;
        } arg ;

    private:
        struct dist_t {
            int   msg;
            float pct ;     // percent
            float step ;
        } ;

        /* Message distribution over mailbox */
        #define md_size 29
        std::vector<struct dist_t> md ;
        static void*  fillMbox( void* arg ) ;
} ;


MailStore::MailStore()
{
    int k ;

    arg.p = this ;
    FILE* f = fopen( "md.txt", "r" ) ;
    while(!feof(f))
    {
        struct dist_t tmp;
        fscanf( f, "%i %f %i", &tmp.msg, &tmp.pct, &tmp.step);
        md.push_back(tmp);
    }
    fclose(f);

    // calculez pasii pentru md
    for( k=0; k < md_size; ++k)
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
        if( arg->mbi > arg->end ) pthread_exit(0) ;
        arg->mbi++ ;
        i = arg->mbi ;
        printf("Current mboxIdx%i\n",i);
        pthread_mutex_unlock( arg->mtx );
        //rc = sendFile() ;
        int k ;
        for( k=0; k < md_size ; k++ )
        {
            if( fmod( i,arg->p->md[k].step) < 1.0 )
            {
                rc = printf("Sending %i mails to user%i\n",arg->p->md[k].msg, i ) ;
                if( !rc ){
                    // If it failed, let other thread try send the mail
                    pthread_mutex_lock( arg->mtx );
                    arg->mbi-- ;
                    pthread_mutex_unlock( arg->mtx ) ;
                }
            }
        }// sendFile
    }
    return 0 ;
}


int main() {
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

    int mboxIdx = 0,i ;
    int maxMbox = 10000;
    pthread_t ta[10] ;
    MailStore* clients[10] ;

    for( i = 0 ; i < 10 ; ++i )
    {
        clients[i] = new MailStore ;
        clients[i]->arg.mbi = mboxIdx ;
        clients[i]->arg.mtx = &mtx ;
        clients[i]->arg.end = maxMbox ;
        pthread_create( &ta[i], NULL, clients[i]->fillMbox, (void*)&clients[i]->arg) ;
    }
    for( i = 0 ; i < 10 ; ++i ) 
        pthread_join( ta[i], NULL ) ;
    for( i = 0; i < 10 ; ++i )
        delete clients[i] ;
    pthread_mutex_destroy(&mtx);  
    return 0;
}
