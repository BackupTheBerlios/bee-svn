#if !defined _TQUEUE_H_
#define _TQUEUE_H_

#include <vector>
#include <queue>
#include <pthread.h>
using std::queue ;
using std::vector ;
using std::string ;


/*!
 * Thread safe queue container.
 **/
template<class T>
class JobQueue : public std::queue<T>
{
    public:
        JobQueue()
        {
            pthread_mutex_init( &qlock, 0 );
        };
        ~JobQueue()
        {
            pthread_mutex_destroy( &qlock );
        };
        void push( const T& val )
        {
//            printf("called %s\n", __FUNCTION__) ;
            pthread_mutex_lock( &qlock ) ;
            std::queue<T>::push( val ) ;
            pthread_mutex_unlock( &qlock ) ;
        }
        const T& front( )
        {
//            printf("called %s\n", __FUNCTION__) ;
            pthread_mutex_lock( &qlock ) ;
            const T& c = std::queue<T>::front( ) ;
            pthread_mutex_unlock( &qlock ) ;
            return c ;
        }
        void pop( )
        {
//            printf("called %s\n", __FUNCTION__) ;
            pthread_mutex_lock( &qlock ) ;
            std::queue<T>::pop( ) ;
            pthread_mutex_unlock( &qlock ) ;
        }
    private:
        pthread_mutex_t qlock ;
} ;//* JobQueue


#endif // include guard
