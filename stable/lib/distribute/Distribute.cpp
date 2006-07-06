#include <cstdlib>
#include <cstdio>
#include "Distribute.h"



Distribute::Smtp::Smtp( const char rcpt_dat[], const char msgsz_dat[] )
{
    FILE* f ;


    f = fopen( rcpt_dat, "r" ) ;
    if(!f){printf("cannot open file %s\n", rcpt_dat ); exit(2); }
    int size ;
    float pct=0,s=0 ;
    while(!feof(f))
    {
        fscanf(f, "%i %f", &size, &pct ) ; // for now, i wont use size
        rcpt_d_.push_back( pct/100 ) ;
    }
    fclose(f);
    s=0;
    printf("rcpt_d_.size = %i\n",rcpt_d_.size() );
    for( int i=0;i<rcpt_d_.size();i++)
        rcpt_cmf_[i] = s = rcpt_d_[i] + s;



    f = fopen( msgsz_dat, "r" ) ;
    if(!f){printf("cannot open file %s\n", msgsz_dat ); exit(2); }
    MsgDist md;
    while(!feof(f))
    {
        fscanf(f, "%i %f", &size, &pct ) ;
        md.size = size ;
        md.pct  = pct/100.0 ;
        msg_d_.push_back(md) ;
    }
    fclose(f);
    s=0;
    for( int i=0;i<msg_d_.size();i++)
        msg_cmf_[i] = s = msg_d_[i].pct + s;

    xsubi_[0] = 0x1234 ^ 12;
    xsubi_[1] = 0x5678 ^ (12 << 8);
    xsubi_[2] = 0x9abc ^ ~12;

}



Distribute::Smtp::~Smtp() {} ;


// check dis for bug. too tired to look at it
int
Distribute::Smtp::rcptTo( int a[30], int maxRcpt )
{
    int i=0,c,n;
    float r = erand48(xsubi_) ;
    while ( (r>rcpt_cmf_[i]) && (i<rcpt_d_.size()) )
    {
        i++;
    }
    for( n=i+1;i>=0;--i)
    {
        c = rand() ;
        a[i] = (int)(c*(1.0*maxRcpt)/RAND_MAX) + 1 ;
    }
    return n ;
}

int
Distribute::Smtp::msgSize()
{
    int i=0;
    float r = erand48(xsubi_) ;
    while ( (r>msg_cmf_[i]) && (i<msg_d_.size()) )
        i++;
    return msg_d_[i].size ;
}

int
Distribute::Smtp::mailFrom(int maxRcpt)
{
    int c = rand() ;
    return (int)(c*(1.0*maxRcpt)/RAND_MAX) + 1 ;
}
