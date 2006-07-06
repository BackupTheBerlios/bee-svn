/*
 *  A POP session
 *  establishes a connection,
 *  receives the banner screen,
 *  authenticates as a local user,
 *  checks a mailbox,
 *  downloads and deletes all messages found,
 *  and disconnects.
 */
user = distribute.pop3login( );
delay = distribute.dataRate( );

Pop3 pop( delay ) ;
pop.connect() ;
pop.login( user, user );
pop.checkMbox( );
pop.retrAll( );
pop.deleAll( ) ;
pop.quit( ) ;


/*  The parameters which determine how many POP sessions are being started each second are UserCount, POP_CHECKS_PER_DAY, REPEATED_POP_CHECKS_PER_DAY,  and PEAK_LOAD_PERCENT:

    * new POP sessions against the "random" pool:
    *

      UserCount * (POP_CHECKS_PER_DAY - REPEATED_POP_CHECKS_PER_DAY) * PEAK_LOAD_PERCENT / 3600
    * new POP sessions against the "retry" pool:
    *

      UserCount * REPEATED_POP_CHECKS_PER_DAY * PEAK_LOAD_PERCENT / 3600
*/

distribute.popSessions( )
{
}

