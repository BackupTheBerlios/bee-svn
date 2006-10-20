#if !defined _DISTRIBUTE_H_
#define _DISTRIBUTE_H_
#if defined DEBUG
    #include "Debug.h"
#endif
#include <vector>
#include "structs.h"

struct MsgDist {
    int size ;
    float pct ;
} ;

using namespace std ;

namespace Distribute {
    class Smtp {
        public:
            Smtp( ) ;
            Smtp( const char rcpt_dat[], const char msgsz_dat[] ) ;
            ~Smtp() ;
            int         mailFrom(int maxRcpt) ;
            int         rcptTo( rcpt_t a[32],int maxRcpt ) ;
            int         msgSize() ;
        private:
            float       msg_cmf_[32] ;
            float       rcpt_cmf_[32] ;
            //u_short     xsubi_[3] ;
            vector<float>   rcpt_d_ ; // rcpt distribution
            vector<MsgDist> msg_d_ ;  // msg size distribution
    } ;


    class Pop3 {
        public:
            Pop3(config_t*) ;
            ~Pop3() ;
            int retry_user() ;
            int random_user();
        private:
            int search( const int p, int left, int rigth ) ;
            int isRetry( const int p ) ;
            int retry_pool[7501] ; // TODO hardcoded
    } ;
};

#endif //_DISTRIBUTE_H_
