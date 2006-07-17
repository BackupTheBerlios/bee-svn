#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>   // srand( getimeofday)
#include <iostream>
#include "tqueue.h"

using namespace std ;



template<class JobType, class Configurator>
class Consumer
{
    public:
        Consumer( )
        {
        };
        Consumer( Configurator* pa )
        {
            prod_arg_ = pa ;
        };
        ~Consumer()
        {};

        void setConf( Configurator* pa )
        {
            prod_arg_ = pa ;
        };

        void jobQueue( JobQueue<JobType*>* jq )
        {
            cout <<"CONS::jq=" <<jq <<endl ;
            jobQueue_ = jq ;
        }
        JobQueue<JobType*>*&
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
        void   consume( JobType* ) ;
        static void*    execute( void* arg ) ;  //!< Threaded function

    private:
        int     count ;     //!< count the items that triggers the producer.
        JobQueue<JobType*>*  jobQueue_ ;
        sem_t*  cons_sem_ ;
        sem_t*  prod_sem_ ;
        Configurator* prod_arg_ ;
} ;



/*!
 * execute : This function will be passed to a pthread_create call.
 * \param[in] a Is casted to Consumer*, to get the "this" pointer.
 * \todo  Implement: Condition P.wait() Condition C.signal()
 **/
 template<class JobType, class Configurator>
    void*
Consumer<JobType,Configurator>::execute( void* a )
{
    cout <<"CONS START" <<endl ;
    extern int  run ;
    JobType*          tmpJob ;
    Consumer* t = (Consumer*)a ;

    while( 1 )
    {
        cout <<"PROD SEM1: " <<t->prod_sem_ <<endl ;
        while( t->jobQueue_->size() <= 0 )
        {
            cout <<"PROD SEM1: " <<t->prod_sem_ <<endl ;
            sem_post( t->prod_sem_ ) ; //! P.signal( ) ;
            cout <<"CONS WAIT:---" <<t->jobQueue_->size() <<endl ;
            sem_wait( t->cons_sem_ );
        }

        tmpJob = t->jobQueue_->front( ) ;
        t->jobQueue_->pop( ) ;

        sem_post( t->prod_sem_ ) ; //! P.signal( ) ;
        //t->consume( tmpJob ) ;
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
