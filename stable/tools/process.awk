#!/usr/bin/awk -f
# Process only timesmtp.csv
# TODO: process timepop3.csv

BEGIN {
    FS = "," ;

    nb_cmd = 8 ;
    cnt[0] = 0 ;        # Just to know what variables are defined :)
    err_cnt[0] = 0 ;        # Just to know what variables are defined :)
    time[0] = 0.0 ;     # Just to know what variables are defined :)

    nb_pcmd = 7 ;
    pcnt[0] = 0 ;        # Just to know what variables are defined :)
    err_pcnt[0] = 0 ;        # Just to know what variables are defined :)
    ptime[0] = 0.0 ;     # Just to know what variables are defined :)

    cmd[0] = "^open" ;
    cmd[1] = "^greet" ;
    cmd[2] = "^mailFrom" ;
    cmd[3] = "^rcptTo" ;
    cmd[4] = "^data" ;
    cmd[5] = "^beginData" ;
    cmd[6] = "^endData" ;
    cmd[7] = "^quit" ;
    # logging errors
    err_cmd[0] = "^ERRopen" ;
    err_cmd[1] = "^ERRgreet" ;
    err_cmd[2] = "^ERRmailFrom" ;
    err_cmd[3] = "^ERRrcptTo" ;
    err_cmd[4] = "^ERRdata" ;
    err_cmd[5] = "^ERRbeginData" ;
    err_cmd[6] = "^ERRendData" ;
    err_cmd[7] = "^ERRquit" ;

    # POP3 commands have a p prefixed, so open becomes popen
    pcmd[0] = "^popen" ;
    pcmd[1] = "^puser";
    pcmd[2] = "^ppass";
    pcmd[3] = "^pstat";
    pcmd[4] = "^pretr";
    pcmd[5] = "^pdele";
    pcmd[6] = "^pquit";
    # logging pop3 error
    err_pcmd[0] = "^ERRpopen" ;
    err_pcmd[1] = "^ERRpuser";
    err_pcmd[2] = "^ERRppass";
    err_pcmd[3] = "^ERRpstat";
    err_pcmd[4] = "^ERRpretr";
    err_pcmd[5] = "^ERRpdele";
    err_pcmd[6] = "^ERRpquit";
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
    print ",-----------------+--------------+--------------+---------------+----------------------------------."
    print "|                 |   Number of  |  Response    |               |  Percentage  Compliant           |"
    print "| Function        |   commands   |  time limit  |     Errors    +-------------+----------+---------+"
    print "|                 |   issued     |  (seconds)   |               |     80%     |   100%   |  120%   |"
    print "|-----------------+--------------+--------------+---------------+-------------+----------+---------+"
for( i=0; i< nb_cmd; i++ )
{
   printf "| SMTP %10s |  %8i    |  %2.3f\t|     %8i  |\n", cmd[i],   cnt[i], time[i], err_cnt[i] ;
}
for( i=0; i< nb_pcmd; i++ )
{
   printf "| POP3 %10s |  %8i    |  %2.3f\t|     %8i  |\n", pcmd[i], pcnt[i], ptime[i] , err_pcnt[i];
}
   print "`------------------------------------------------------------------------------'"


}
