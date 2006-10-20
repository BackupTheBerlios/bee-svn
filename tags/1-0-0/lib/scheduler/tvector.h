#if !defined _VECTORR_H_
#define _VECTORR_H_

#include <vector>
#include <pthread.h>
using std::queue ;
using std::vector ;
using std::string ;


/*!
 * Thread safe queue container.
 **/
template<class T>
class vector_r : public std::vector<T>
{
    public:
        vector_r()
        {
            pthread_mutex_init( &qlock, 0 );
        };
        ~vector_r()
        {
            pthread_mutex_destroy( &qlock );
        };
        void push_back( const T& val )
        {
//            printf("called %s\n", __FUNCTION__) ;
            pthread_mutex_lock( &qlock ) ;
            std::queue<T>::push_back( val ) ;
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
