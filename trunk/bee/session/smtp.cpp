/*
 * An SMTP session connects to the server,
 * receives the banner screen,
 * transmits sender,
 * recipients and
 * contents of a message,
 * and disconnects
 * . There are two types of SMTP sessions,
 * and there a slight difference how the
 * configuration parameters affect each one. */


class SmtpSession { 
    SmtpSession() ;
    ~SmtpSession() ;
} ;
/*
 *      For an SMTP session simulating a message sent by a local user,
 *      the configuration parameters have the following influence.
 - The number of recipients is determined according to MSG_RECP_DISTRIBUTION.
 - The actual target accounts are selected randomly from the range of available
 * user accounts.
 - The decision of whether a recipient is to be in the local or remote domain
 * is made according to MSG_DESTINATION_LOCAL_PERCENT.
 - The length of the message to be sent is based on MSG_SIZE_DISTRIBUTION.
 * The speed at which the connection is handled
 *  - how much delay is inserted in between steps -
 -  is based on DATA_RATE_DISTRIBUTION.
 - Every SMTP session will transmit exactly one message.
*/
SmtpSession::local(  )
{
    vector< Rcpt> rcpts ;
    int delay ;
    distribute.dataRate( &delay ) ;

    // un constructor ia ca parametru delay, si-l paseaza lui smtp.
    // Smtp, inainte de a incepe lucrul, face un sleep(delay)
    Smtp smtp( delay ) ;
    smtp.open() ;
    smtp.greet() ;
    smtp.mailFrom( );
    distribute.rcpt( &rcpts, MSG_DESTINATION_LOCAL_PERCENT ) ;
    smtp.rcptTo( rcpts ) ;
    distribute.msgSize( &msg_size ) ;
    smtp.data(msg_size) ;
    smtp.quit() ;
}

/*
 *      For an SMTP session simulating incoming traffic from another mail
 *      server, the configuration parameters are applied as follows:
 - The number of recipients is determined according to MSG_RECP_DISTRIBUTION.
 - The actual target accounts are selected randomly from the range of available
 * user accounts.
 - All recipients are assumed to be local..
 - The length of the message to be sent is based on MSG_SIZE_DISTRIBUTION.
 - The connection speed is handled at full speed, without artificial delays..
 - Every SMTP session will transmit exactly one message.
 */
SmtpSession::incomming( )
{
    Smtp smtp(0) ;
    smtp.open() ;
    smtp.greet() ;
    smtp.mailFrom( );
    distribute.rcpt( &rcpts ) ;
    smtp.rcptTo( rcpts ) ;
    distribute.msgSize( &msg_size ) ;
    smtp.data( msg_size ) ;
    smtp.quit( ) ;
}


/*
 *      SMTP sessions simulating local users' messages are started according to
 *      the formula
 * new sessions per second = UserCount * MSG_SENT_PER_DAY * PEAK_LOAD_PERCENT / 3600
 *
 *      SMTP sessions simulating incoming traffic from other mail servers are
 *      started according to the formula
 * new sessions per second = UserCount * MSG_SENT_PER_DAY * PEAK_LOAD_PERCENT / 3600 * (1 - MSG_DESTINATION_LOCAL_PERCENT)
 */
distribute.sessions( )
{
}
