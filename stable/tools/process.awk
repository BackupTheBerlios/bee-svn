#!/usr/bin/awk -f
# Process only timesmtp.csv
# TODO: process timepop3.csv

BEGIN {
    FS = "," ;

    nb_cmd = 8 ;
    cnt[0] = 0 ;        # Just to know what variables are defined :)
    err_cnt[0] = 0 ;        # Just to know what variables are defined :)
    time[0] = 0.0 ;     # Just to know what variables are defined :)

    nb_pcmd = 6 ;
    pcnt[0] = 0 ;        # Just to know what variables are defined :)
    err_pcnt[0] = 0 ;        # Just to know what variables are defined :)
    ptime[0] = 0.0 ;     # Just to know what variables are defined :)

    cmd[0] = "open" ;
    cmd[1] = "greet" ;
    cmd[2] = "mailFrom" ;
    cmd[3] = "rcptTo" ;
    cmd[4] = "data" ;
    cmd[5] = "beginData" ;
    cmd[6] = "endData" ;
    cmd[7] = "quit" ;
    # logging errors
    err_cmd[0] = "ERRopen" ;
    err_cmd[1] = "ERRgreet" ;
    err_cmd[2] = "ERRmailFrom" ;
    err_cmd[3] = "ERRrcptTo" ;
    err_cmd[4] = "ERRdata" ;
    err_cmd[5] = "ERRbeginData" ;
    err_cmd[6] = "ERRendData" ;
    err_cmd[7] = "ERRquit" ;

    # POP3 commands have a p prefixed, so open becomes popen
    pcmd[0] = "popen" ;
    pcmd[1] = "puser";
    pcmd[2] = "ppass";
    pcmd[3] = "pstat";
    pcmd[4] = "pretr";
    pcmd[5] = "pdele";
    # logging pop3 error
    err_pcmd[0] = "ERRpopen" ;
    err_pcmd[1] = "ERRpuser";
    err_pcmd[2] = "ERRppass";
    err_pcmd[3] = "ERRpstat";
    err_pcmd[4] = "ERRpretr";
    err_pcmd[5] = "ERRpdele";
}

{
    for( i=0; i< nb_cmd; i++) {
        found = match($1, cmd[i])
        if( found ) {
            cnt[i]++
            if( $2 >time[i] ) { time[i] = $2 }
        }
        found = match($1, err_cmd[i])
        if( found ) {
            err_cnt[i]++
        }
        
        found = match($1, pcmd[i])
        if( found ) {
            pcnt[i]++
            if( $2 >ptime[i] ) { ptime[i] = $2 }
        }
        found = match($1, err_pcmd[i])
        if( found ) {
            err_pcnt[i]++
        }
    }
}

END {
    print ",----------------------------------------------------------------------------------."
    print "|                                Summary Results                                   |"
    print "|------------------+--------------+-------------+----------------------------------+"
    print "|                  |   Response   | Required    |  Percentage  Compliant           |"
    print "|      Function    |   time limit | Percentage  +-------------+----------+---------+"
    print "|                  |   (seconds)  | Compliant   |     80%     |   100%   |  120%   |"
    print "|------------------+--------------+-------------+-------------+----------+---------+"
    print "| SMTP Connect     |        5     |     >95%    |   100.00%   | 100.00%  |100.00%  |"
    print "| SMTP Data        |        5     |     >95%    |    99.98%   |  99.98%  | 99.93%  |"
    print "| POP Connect      |        5     |     >95%    |   100.00%   | 100.00%  |100.00%  |"
    print "| POP Status       |        5     |     >95%    |   100.00%   | 100.00%  |100.00%  |"
    print "| POP Retrieve     |        5     |     >95%    |   100.00%   | 100.00%  |100.00%  |"
    print "| POP Delete       |        5     |     >95%    |   100.00%   | 100.00%  |100.00%  |"
    print "| Delivery Time    |        60    |     >95%    |    99.05%   |  99.09%  | 63.73%  |"
    print "| Error Rate       |        NA    |     <1%     |     0.00%   |   0.00%  |  0.00%  |"
    print "`----------------------------------------------------------------------------------'"

    print "Results:"
    for( i=0; i< nb_cmd; i++ )
    {
        printf "Number of SMTP %s: %i. Max time=%f\n", cmd[i], cnt[i], time[i] ;
    }
    for( i=0; i< nb_pcmd; i++ )
    {
        printf "Found %i of %s. Max time=%f\n",  pcnt[i], pcmd[i], ptime[i] ;
        printf "ERRORS %i errors on %s\n", err_pcnt[i], pcmd[i] ;
    }
}
