#if !defined _STRUCTS_H_
#define _STRUCTS_H_

#include <netinet/in.h>

typedef struct config {
    int     init_only ;
    char*   smtp_server ;
    int     smtp_port ;
    char*   pop3_server ;
    int     pop3_port ;
    char*   sink_server ;
    int     sink_port ;
    char*   local_domain ;
    char*   remote_domain ;
    char*   user_prefix ;
    char*   user_passwd ;
    int     user_start ;
    int     user_end ;
    int     init_rest ;
    int     threads ;
    int     clients ;
    int     span ;
    int     warmup ;
    int     rampdown ;
    int     users ;
    int     resolution ;
    int     is_filled ;
    int     msg_per_day ;
    int     pop_chk ;
    int     msg_size_avg ;
    int     msg_distr ;     // average recipients per message
    int     msg_rcvd ;      // average recipients per message
    int     msg_local ;     // % of mail to/from remote addresses
    int     rate_distr ;    // % of users using modems (56Kbit)
    int     pk_load_pct ;   // % of daily activities during busiest hour
    int     repeat_chk ;
    void*   ths ;           // LoadGen::Smtp
    void*   pths ;          // LoadGen::Pop3 //TODO init it pls
    sockaddr_in* dest ;
    sockaddr_in* pdest ;    // pop3server resolved
    config() {    // default values
       init_only    = 0;
       smtp_server  = "localhost" ;
       smtp_port    = 25 ;
       pop3_server  = "localhost" ;
       pop3_port    = 110 ;
       sink_server  = "localhost" ;
       sink_port    = 25 ;
       local_domain = "localdomain" ;
       remote_domain= "remotedomain" ;
       user_prefix  = "user" ;
       user_passwd  = "user" ;
       user_start   = 1 ;
       user_end     = 100 ;
       init_rest    = 12 ;
       threads      = 1 ;
       clients      = 1 ;
       span         = 1 ;
       warmup       = 1 ;
       rampdown     = 1 ;
       users        = 100 ;
       resolution   = 1 ;
       is_filled    = 0 ;
       pop_chk      = 4 ;   // mailbox checks per day
       msg_distr    = 2 ;   // average recipients per message
       msg_per_day  = 2 ;   // add to usage()
       msg_size_avg = 25 ;  // average message size in KB
       msg_rcvd     = 4 ;   // average recipients per message
       msg_local    = 90 ;  // % of mail to/from remote addresses
       rate_distr   = 90 ;  // % of users using modems (56Kbit)
       pk_load_pct  = 15 ;  // % of daily activities during busiest hour
       repeat_chk   = 300;  // has a value between 300 and 600 seconds
    } ;
} config_t ;

#endif //_STRUCTS_H_
