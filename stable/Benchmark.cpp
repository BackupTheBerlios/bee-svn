#include "Benchmark.h"


/**
 * param[in] load at which the run will be made
 * param[in] is_filled If the MailStore was pre-populated. */
void
Benchmark::run( double load, bool& is_filled )
{
    if( !is_filled )
    {
        ms_.init( SMTP_SERVER ,SMTP_PORT, USER_END-USER_START + 1 ,10 ) ;
        is_filled = true ;
    }

    smtpGen_.init( ) ;
    pop3Gen_.init( ) ;
    sysInfo_.init( ) ;

    cron.refresh( 1 ) ;       //! set the time[sec] at which timeList is inspected

    // these will wait, untill cron will call them to run some jobs
    smtpGen_.run() ;
    pop3Gen_.run() ;
    sysInfo_.run() ;

    cron.start() ;      // Cron does all the work. At given times, it calles a spawned smtpGen to do a smtpSession

    while( cron.timer.elapsed() <= TEST_DURATION )
    {
        ;
    }
    cron.stop() ;
    smtpGen_.stop( ) ;    // asta ar tb sa dump-uiasca rezultatele
    pop3Gen_.stop( ) ;    // asta ar tb sa dump-uiasca rezultatele
    sysInfo_.stop( ) ;
    is_filled = false ;
}


//  TODO
// Găseşte locul unde tb sa fac asta :
//      cron.addTime( rateGen.exponential() ) ; // adaug un timp nou de cate ori expira altul( un job a fost rulat)
// asta ar tb sa culeaga timpii de pe sock, si sa le trimita la un server care le scrie
// serverul poate sa fie local, sau remote, in cazul in care 
// astea ar tb sa spawneze/fork threadurile necesare ca sa ruleze separat the mainThread
