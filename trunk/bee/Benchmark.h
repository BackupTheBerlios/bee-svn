#include "Timer.h"
#include "SysInfo.h"

class Benchmark {
    public:
        Benchmark() ;
        ~Benchmark() ;
        void run( float load, bool& is_init ) ;
    private:
        LoadGen::Smtp smtpGen_ ;
        LoadGen::Pop3 pop3Gen_ ;
        MailStore ms_ ;
        SysInfo sysInfo_ ;
        Timer timer_ ;
};


