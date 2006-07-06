#include "Distribute.h"

Distribute::Smtp::Smtp( const char rcpt_dat[], const char msgsz_dat[] )
{
    FILE f ;
    f = fopen( rcpt_dat, "r" ) ;
    if(!f){printf("cannot open file %s\n", rcpt_dat ); exit(2); }

    while(!feof(f))
    {
        fscanf(f, "", ) ;
        rcpt_distr_.push_back() ;
    }
    fclose(f);

    f = fopen( msgsz_dat, "r" ) ;
    if(!f){printf("cannot open file %s\n", rcpt_dat ); exit(2); }

    while(!feof(f))
    {
        fscanf(f, "", ) ;
        msg_distr_.push_back() ;
    }
    fclose(f);
}
Distribute::Smtp::~Smtp() {} ;

Distribute::Smtp::rcptTo() {} ;
Distribute::Smtp::msgSize() {} ;
Distribute::Smtp::mailFrom() {} ;
