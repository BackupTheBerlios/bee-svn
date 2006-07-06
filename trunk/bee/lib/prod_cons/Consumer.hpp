#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>   // srand( getimeofday)
#include <iostream>
#include "tqueue.h"

using namespace std ;



template<class T, class Arg>
class Consumer
{
    public:
        Consumer( )
        {
        };
        Consumer( Arg* pa )
        {
            prod_arg_ = pa ;
        };
        ~Consumer()
        {};

        void setConf( Arg* pa )
        {
            prod_arg_ = pa ;
        };

        void jobQueue( JobQueue <T*>* jq )
        {
            jobQueue_ = jq ;
        }
        JobQueue<T*>*&
        jobQueue( )
        {
            return jobQueue_ ;
        }
        void prod_sem( sem_t* cond )
        {
            prod_sem_ = cond ;
        }
        void cons_sem( sem_t* cond )
        {
            cons_sem_ = cond ;
        }
        void   consume( T* ) ;
        static void*    execute( void* arg ) ;  //!< Threaded function

    private:
        int     count ;     //!< count the items that triggers the producer.
        JobQueue<T*>*  jobQueue_ ;
        sem_t*  cons_sem_ ;
        sem_t*  prod_sem_ ;
        Arg* prod_arg_ ;
} ;



/*!
 * execute : This function will be passed to a pthread_create call.
 * \param[in] a Is casted to Consumer*, to get the "this" pointer.
 * \todo  Implement: Condition P.wait() Condition C.signal()
 **/
 template<class T, class Arg>
    void*
Consumer<T,Arg>::execute( void* a )
{
    cout <<"CONS START" <<endl ;
    extern int  run ;
    T*          tmpJob ;
    Consumer* t = (Consumer*)a ;

    while( 1 )
    {
        while( t->jobQueue_->size() <= 0 )
        {
            cout <<"PROD SEM: " <<t->prod_sem_ <<endl ;
            sem_post( t->prod_sem_ ) ; //! P.signal( ) ;
            cout <<"CONS WAIT:---" <<t->jobQueue_->size() <<endl ;
            sem_wait( t->cons_sem_ );
        }

        tmpJob = t->jobQueue_->front( ) ;
        t->jobQueue_->pop( ) ;

        sem_post( t->prod_sem_ ) ; //! P.signal( ) ;
        t->consume( tmpJob ) ;
    }
}//* Consumer::run







/*!
 * Producer : Generates Jobs which will be consumed by Consumer.
 *
 * JobQueue and cond variables are to be passed between Producer and
 * Consumer
 * The way these Jobs are produced, should not be Producers' concern,
 * so a virtual function "produce()" have to be implemented by each
 * type of producer.
 *
 * The assignement of the Jobs is done using a <queue> of pointers to Jobs.
 * In this way, Producer only have to be concerned about syncing the
 * queue, and signaling the consumers.
 *
 * The extern layer should deal with the contruction and destruction
 * of the jobQueue */
