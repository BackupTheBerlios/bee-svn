#if !defined _LOAD_GEN_H_
#define _LOAD_GEN_H_
class LoadGen {
    public:
        LoadGen() ;
        ~LoadGen() ;
        void initSmtp();
        void initPop3();
        void stopSmtp() ;
        void stopPop3() ;
        void runSmtp() ;
        void runPop3() ;
    private:
        // array-ul de threaduri poate fi inlocuit cu un thread-pool
        int numThreads ;
        pthread_t smtpTh[50];
        pthread_t pop3Th[50];
        RateGen rateGen_(mean);

} ;
#endif // _LOAD_GEN_H_
