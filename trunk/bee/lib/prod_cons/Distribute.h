#if !define _DISTRIBUTE_H_
#define _DISTRIBUTE_H_

namespace Distribute {
    class Smtp {
        public:
            Smtp( const char rcpt_dat[], const char msgsz_dat[] ) ;
            ~Smtp() ;
            mailFrom() ;
            rcptTo() ;
            msgSize() ;
        private:
            vector<float> rcpt_distr_ ;
            vector<float> msg_distr_ ;
    } ;
};
#endif //_DISTRIBUTE_H_
