
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>   // srand( getimeofday)

#include "tqueue.h" //! hopefully a threaded queue. :)

#include <iostream>
using namespace std ;


template<class T, class Arg>
class Producer
{
    public:
        Producer( Arg* pa ) : count(10) { prod_arg_ = pa ; };
        Producer( ) : count(10) { };
        ~Producer() {};
        void jobQueue( JobQueue <T*>* jq )
        {
            jobQueue_ = jq ;
        }

        void config( Arg* pa) { prod_arg_ = pa ; } ;
        JobQueue<T*>*& jobQueue( )
        {
            return jobQueue_ ;
        }
        void prod_sem( sem_t* cond )
        {
            prod_sem_= cond ;
        }
        void cons_sem( sem_t* cond )
        {
            cons_sem_ = cond ;
        }
        static void*    execute( void* arg ) ;  //!< Threaded function
        T*   produce( Arg* ) ;

    private:
        int     count ;     //!< count the items that triggers the producer.
        JobQueue<T* >*  jobQueue_ ;
        sem_t*  prod_sem_ ;
        sem_t*  cons_sem_ ;
        Arg* prod_arg_ ;
} ;



/*!
 * execute : This function will be passed to a pthread_create call.
 * \param[in] a Is casted to Producer*, to get the "this" pointer.
 * \todo  Implement: Condition P.wait() Condition C.signal()
 **/
 template<class T, class Arg>
    void*
Producer<T,Arg>::execute( void* a )
{
    cout <<"++PRODUCE: START" <<endl ;
    extern int  run ;
    T*          tmpJob ;
    Producer* t = (Producer*)a ;

    while( 1 )
    {
        tmpJob = t->produce( t->prod_arg_ ) ;
        while( t->jobQueue_->size() > 5 )
        {
            cout <<"++PROD SMPH: " <<t->prod_sem_ <<endl ;
            cout <<"++PROD Qsize:" <<t->jobQueue_->size() <<endl ;
            sem_wait( t->prod_sem_ ); 
        }

        t->jobQueue_->push( tmpJob ) ;
        sem_post( t->cons_sem_ ) ;//! C.signal
    }
}//* Producer::run
