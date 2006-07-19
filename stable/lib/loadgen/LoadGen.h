#if !defined _LOAD_GEN_H_
#define _LOAD_GEN_H_
#include "distribute/Distribute.h"
#include "../scheduler/Scheduler.h"
#include "../../structs.h"
#include <pthread.h>

namespace LoadGen {
    class Smtp {
        public:
            Smtp() ;
            ~Smtp() ;
            void   init( config_t* cfg ) ;
            void   semaphore(sem_t* sem ) { sem_ = sem  ; } ;
            void   stop() ;
            void   run() ;
            static void* worker(void*) ;
        private:
            config_t*   cfg_ ;
            sem_t*      sem_ ;
            Distribute::Smtp* smtpDistr;
            Scheduler   cron ;
    } ;

    class Pop3 {
        public:
            Pop3() ;
            ~Pop3() ;
            void   init( config_t* cfg ) ;
            void   semaphore(sem_t* sem ) { sem_ = sem  ; } ;
            void   stop() ;
            void   run() ;
            static void* worker(void*) ;
        private:
            config_t*   cfg_ ;
            sem_t*      sem_ ;
            Distribute::Pop3* pop3Distr;
            Scheduler   cron ;
            unsigned short   xsubi_[3] ;
    } ;
};
#endif // _LOAD_GEN_H_
