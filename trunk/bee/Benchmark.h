#include "Timer.h"
#include "LoadGen.h"
#include "SysInfo.h"

class Benchmark {
    public:
        Benchmark(){} ;
        ~Benchmark(){} ;
        Scheduler::Cron cron( semaphore_ ) ; // donno why is this needed
        void run( float load, bool& is_init ) ;

    private:
        LoadGen::Smtp smtpGen_ ;
        LoadGen::Pop3 pop3Gen_ ;
        MailStore ms_ ;
        SysInfo sysInfo_ ;
        Timer timer_ ;
        semaphore_t* semaphore_ ;
};


