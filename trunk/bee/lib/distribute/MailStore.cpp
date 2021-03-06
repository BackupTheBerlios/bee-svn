#if !defined _MAIL_STORE_
#define _MAIL_STORE_

#if defined DEBUG
    #include "Debug.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MailStore.h"
#include "smtp/Protocol.h"
#include <unistd.h>

MailStore::MailStore(const char* fname)
{
    int k ;

    arg.p = this ;
    FILE* f = fopen( fname, "r" ) ;
    if( !f ) { printf("Can't open %s\n",fname ) ; exit(1) ; }
    while(!feof(f))
    {
        struct dist_t tmp;
        fscanf( f, "%i,%f", &tmp.msg, &tmp.pct );
        md.push_back(tmp);
    }
    fclose(f);

    // calculez pasii pentru md
    for( k=0; k < md.size(); ++k)
    {
        md[k].step = 100.0/md[k].pct ;
        debug("pct[%f]\t-- step[%f]", md[k].pct, md[k].step ) ;
    }
}

void
MailStore::sendMails( const char* host, const int port, const int userIdx, const int quantity )
{
    char user[512] ={0} ;
    sprintf( user, "user%i", userIdx ) ; 

    Smtp::Protocol smtp ;
    for( int i=0; i< quantity; ++i)
    {
        try {
            smtp.open( host, port );
            smtp.greet("ehlo cucu");
            smtp.mailFrom("<>");
            smtp.rcptTo( user ) ;
            smtp.beginData();
            smtp.sendFile( "./data/populate.eml") ;
            smtp.endData();
            smtp.quit() ;
            debug("Sent %i mails out of %i, to %s", i+1, quantity, user) ;
        }catch( Socket::Exception& e )
        {
            --i ;// mail not sent: try again
            smtp.close() ;
            fprintf( stderr, "Smtp ProtocolError: %s\n", e.what() ) ;
            debug( "Smtp ProtocolError: %s", e.what() ) ;
        }
    }
}


/**
 * param[in] delay The number of miliseconds to wait after each mail is sent.*/
    void*
MailStore::fillMbox( void* t )
{
    struct targ* arg = (struct targ*) t ;
    int i = 0,z,rc=0 ;

    for(;;)
    {
        pthread_mutex_lock( arg->mtx );
        {
            i = arg->mbi ;
            arg->mbi++ ;
            if( arg->mbi%1000 == 0 )
            {
                printf("DND: sleeping 5 seconds\n");
                sleep(5); // sleep for 10 sec, after 1000 mboxes filled
            }
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
                    debug("all done: exit");
                    pthread_exit(0);
                }
                z=arg->mbf++ ;
                pthread_mutex_unlock( arg->mtx ) ;
                debug("User%i will have %i mails", z, arg->p->md[k].msg ) ;
                arg->p->sendMails( arg->host, arg->port, z, arg->p->md[k].msg ) ;
            }
        }
    }
    return 0 ;
}



int
MailStore::init( const char* host, const int port, const int maxMbox, const int threads)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

    int         i=0, rc=0, k=0 ;
    pthread_t   ta[threads], t ;
    struct      dist_t tmp ;

    arg.mbi = 0 ;
    arg.mbf = 0 ;
    arg.mtx = &mtx ;
    arg.end = maxMbox ;
    arg.host= (char*)host ;
    arg.port= port ;

    for( i = 0 ; i < threads ; ++i )
    {
        rc = pthread_create( &t, NULL, fillMbox, (void*)&arg) ;
        if( rc == 0 ) ta[k++] = t ;
        else printf( "Error creating thread %i\n", i ) ;
    }
    for( i = 0 ; i < k ; ++i )
        pthread_join( ta[i], NULL ) ;


    pthread_mutex_destroy(&mtx) ;
    return 0;
}

#endif // _MAIL_STORE_
