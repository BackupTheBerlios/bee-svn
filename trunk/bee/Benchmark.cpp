/* 
 * Accordingly, a SPECmail2001 benchmark run consists of a series of warm-up,
 * measurement and cool-down phases:
 *
 * Only the TRANSACTION LOAD is varied between these three points. The
 * pre-populated mail store needs to match the 100% load level (larger is OK),
 * and does not vary for the 80% or the 120% test case. 
 */

class Benchmark {
    public:
        Benchmark() ;
        ~Benchmark() ;
        void run() ;
    private:
    MailStore ms_ ;
};


/**
 * SpecMail mentions abt 3 loads, 80%, 100%, and 120%,
 * so a valid call would be run(80) .
 * param[in] load at which the run will be made.
 * */
void
run(float load, bool& is_init )
{
    if( !is_init )
    {
        ms_.init( SMTP_SERVER ,SMTP_PORT, USER_END-USER_START + 1 ,10 ) ;
        is_init = true ;
    }
    while( elapsed < TEST_DURATION )  
    {

        distribute.smtpSessions() ;
        distribute.pop3Sessions() ;
    }
    is_init = false ;
}
