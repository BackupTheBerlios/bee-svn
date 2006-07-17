#if !defined _DISTRIBUTE_H_
#define _DISTRIBUTE_H_
#include <vector>

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
            int         rcptTo( int a[30],int maxRcpt ) ;
            int         msgSize() ;
        private:
            float       msg_cmf_[30] ;
            float       rcpt_cmf_[30] ;
            u_short     xsubi_[3] ;
            vector<float>   rcpt_d_ ; // rcpt distribution
            vector<MsgDist> msg_d_ ;  // msg size distribution
    } ;
    class Pop3 {
    } ;
};

#endif //_DISTRIBUTE_H_
