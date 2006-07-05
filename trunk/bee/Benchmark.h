#include "Timer.h"
#include "SysInfo.h"

class Benchmark {
    public:
        Benchmark() ;
        ~Benchmark() ;
        void run( float load, bool& is_init ) ;
    private:
        LoadGen loadGen_ ;
        MailStore ms_ ;
        SysInfo sysInfo_ ;
        Timer timer_ ;
};


