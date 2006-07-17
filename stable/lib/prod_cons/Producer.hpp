
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>   // srand( getimeofday)

#include "tqueue.h" //! hopefully a threaded queue. :)

#include <iostream>
using namespace std ;


template<class JobType, class Configurator>
class Producer
{
    public:
        Producer( Configurator* pa ) : count(10)
        {   prod_arg_ = pa ;
            sem_init( &sem_, 0,0 ) ; 
        };
        
        Producer( ) : count(10)
        {
            sem_init( &sem_, 0,0 ) ; 
        };

        ~Producer() {};

        void set_ipc( JobQueue<JobType*>* jq, sem_t* consumer_sem )
        {
            cout << "PROD::jq="<< jq <<endl ;
            jobQueue_ = jq ;
            cons_sem_ = consumer_sem ;
        }
        
        sem_t* semaphore( )
        {
            return &sem_ ;
        }

        JobType*   produce( Configurator* ) ;
        static void*    execute( void* arg ) ;  //!< Threaded function

    private:
        int     count ;     //!< count the items that triggers the producer.
        sem_t   sem_ ;
        sem_t*  cons_sem_ ;
        Configurator* prod_arg_ ;
        JobQueue<JobType* >*  jobQueue_ ;
} ;



/*!
 * execute : This function will be passed to a pthread_create call.
 * \param[in] a Is casted to Producer*, to get the "this" pointer.
 * \todo  Implement: Condition P.wait() Condition C.signal()
 **/
 template<class JobType, class Configurator>
    void*
Producer<JobType,Configurator>::execute( void* a )
{
    cout <<"++PRODUCE: START" <<endl ;
    extern int  run ;
    JobType*          tmpJob ;
    Producer* t = (Producer*)a ;

    while( 1 )
    {
        tmpJob = t->produce( t->prod_arg_ ) ;
        while( t->jobQueue_->size() > 5 )
        {
            cout <<"++PROD SMPH: " <<t->sem_ <<endl ;
            cout <<"++PROD Qsize:" <<t->jobQueue_->size() <<endl ;
            sem_wait( &t->sem_ ); 
        }

        t->jobQueue_->push( tmpJob ) ;
        sem_post( t->cons_sem_ ) ;//! C.signal
    }
}//* Producer::run
