/* 
 * Accordingly, a SPECmail2001 benchmark run consists of a series of warm-up,
 * measurement and cool-down phases:
 *
 * Only the TRANSACTION LOAD is varied between these three points. The
 * pre-populated mail store needs to match the 100% load level (larger is OK),
 * and does not vary for the 80% or the 120% test case. 
 */
#include "Benchmark.h"

Benchmark::Benchmark()
{
    // initialize stuff
}

Benchmark::~Benchmark()
{
}


/**
 * SpecMail mentions abt 3 loads, 80%, 100%, and 120%,
 * so a valid call would be run(80) .
 * param[in] load at which the run will be made.
 * This stage should spawn the necessary threads, and do
 * the needed preparations.
 */
 void
Benchmark::run(float load, bool& is_init )
{
    if( !is_init )
    {
        ms_.init( SMTP_SERVER ,SMTP_PORT, USER_END-USER_START + 1 ,10 ) ;
        is_init = true ;
    }
    loadGen_.initStmp( ) ;
    loadGen_.initPop3( ) ;
    sysInfo_.init( ) ;
    timer_.start() ;        // after all is done, start the timer
    // astea ar tb sa spawneze/fork threadurile necesare
    // ca sa ruleze separat the mainThread
    while( timer_.elapsed() < TEST_DURATION )
    {
        loadGen_.runSmtp();   // asta ar tb sa culeaga timpii de pe sock, si sa le trimita la un server care le scrie
        loadGen_.runPop3();   // serverul poate sa fie local, sau remote, in cazul in care 
        sysInfo_.run( ) ;
    }
    timer_.stop() ;
    loadGen_.stopStmp( ) ;    // asta ar tb sa dump-uiasca rezultatele
    loadGen_.stopPop3( ) ;    // asta ar tb sa dump-uiasca rezultatele
    sysInfo_.stop( ) ;
    is_init = false ;
    // TODO : o modalitate sa calculez QoS si sa pun sysinfos la punct
}


/*
 * smtp.greet( );
 * Clasa Smtp i se seteaza un obiect de tip results( se poate gasi alt nume :)  care
 * contine numarul de calluri catre functii, si timpii lor.
 * ăsta e modul in care face httpperf, adica ţine Rezultatele in memorie,
 * ca la sfârşit sa facă un dump al rezultatelor
