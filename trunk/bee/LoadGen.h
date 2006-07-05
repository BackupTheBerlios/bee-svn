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
} ;
#endif // _LOAD_GEN_H_
