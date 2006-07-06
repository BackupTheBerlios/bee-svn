#include <iostream>
#include "tqueue.h"

using std::cout ;
using std::endl ;

/*!
 * Test thread safenes of tqueue.
 *
 * Create two threads, one that pushes items into queue,
 * and other that pops items from it.
 */

void* pusher( void* a )
{
    JobQueue<int>* jq = (JobQueue<int>*)a ;
    while( true )
    {
        jq->push( 1 ) ;
    }
    return 0 ;
}

void* popper( void* a )
{
    JobQueue<int>* jq = (JobQueue<int>*)a ;
    while( true )
    {
        if( !jq->empty() )
        {
            cout << jq->front() <<endl ;
            jq->pop() ;
        }
    }
    return 0 ;
}

int main()
{
    JobQueue<int> tq ;
    pthread_t t1, t2 ;
    pthread_create( &t1, 0, pusher, &tq ) ; //pusher
    pthread_create( &t2, 0, popper, &tq ) ; //popper
    pthread_join( t1, 0) ;
    pthread_join( t2, 0) ;
    return 0 ;
}
