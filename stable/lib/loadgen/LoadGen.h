#if !defined _LOAD_GEN_H_
#define _LOAD_GEN_H_
#include "prod_cons/Producer.hpp"
#include "prod_cons/Consumer.hpp"
#include "prod_cons/tqueue.h"
#include "distribute/Distribute.h"
#include "scheduler/Scheduler.h"
#include <pthread.h>





class SmtpConfig {
    public:
        int userIdx ;
        SmtpConfig( ) { userIdx = 10 ; };
};



struct Job {
    int usrIdx ;
    int domIdx ;
    int rcpts ;
    int msg_sz ;
    int rcptList[32] ; // round up to 32
} ;


void tick(union sigval sigval) ;
void* thread_fun(void*) ;






namespace LoadGen {
    class Smtp {
        public:
            Smtp() ;
            ~Smtp() ;
            void init( int nbcL, int nbtR, int refresH, int tduratioN ) ;
            void stop() ;
            void run() ;
            static void tick(union sigval sigval ) ;
        private:
            int     nbcl ;
            int     nbtr ;
            int     refresh ;
            int     tduration ;
            sem_t   sched_sem ;
            sem_t   prod_sem ;
            sem_t   cons_sem ;
            //unsigned int    elapsed ;
            pthread_t       c[40] ; // todo
            pthread_t       p ;
            SmtpConfig      smtpConfig ;
            JobQueue<Job*>  jq ;
            Distribute::Smtp* smtpDistr;
            Consumer<Job, SmtpConfig>   consumer[40] ; //todo
            Producer<Job, Distribute::Smtp>   producer ;
    } ;
};
#endif // _LOAD_GEN_H_
