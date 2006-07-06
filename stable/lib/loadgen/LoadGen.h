#if !defined _LOAD_GEN_H_
#define _LOAD_GEN_H_

namespace LoadGen {
    class Smtp {
        public:
            Smtp() ;
            ~Smtp() ;
            void init();
            void stop() ;
            void run() ;
        private:
            // array-ul de threaduri poate fi inlocuit cu un thread-pool
            int noThreads ;
            pthread_t threads[50];
            RateGen rateGen_(mean);
    } ;

    class Pop3 {
        public:
            Pop3() ;
            ~Pop3() ;
            void init();
            void stop() ;
            void run() ;
        private:
            // array-ul de threaduri poate fi inlocuit cu un thread-pool
            int noThreads ;
            pthread_t threads[50];
            RateGen rateGen_(mean);
    } ;
};
#endif // _LOAD_GEN_H_
