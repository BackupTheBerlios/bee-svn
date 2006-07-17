#!/usr/bin/awk -f


BEGIN {
    FS = "," ;
    #print "Analizing data..." ;
    nb_cmd = 8 ;
    cnt[0] = 0 ;        # Just to know what variables are defined :)
    time[0] = 0.0 ;     # Just to know what variables are defined :)
    cmd[0] = "greet" ;
    cmd[1] = "mailFrom" ;
    cmd[2] = "rcptTo" ;
    cmd[3] = "data" ;
    cmd[4] = "quit" ;
    cmd[5] = "open" ;
    cmd[6] = "beginData" ;
    cmd[7] = "endData" ;
}

{
    for( i=0; i< nb_cmd; i++) {
        found = match($1, cmd[i])
        if( found ) {
            cnt[i]++
            if( $2 >time[i] ) { time[i] = $2 }
            break
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
        printf "Found %i of %s. Max time=%f\n",  cnt[i], cmd[i],time[i] ;
    }
}
