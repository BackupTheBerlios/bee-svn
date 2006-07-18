#include <cstdlib>
#include <cstdio>
#include "Distribute.h"
#include "../../structs.h"



Distribute::Smtp::Smtp( const char rcpt_dat[], const char msgsz_dat[] )
{
    FILE* f ;


    f = fopen( rcpt_dat, "r" ) ;
    if(!f){printf("cannot open file %s\n", rcpt_dat ); exit(2); }
    int size ;
    float pct=0,s=0 ;
    while(!feof(f))
    {
        fscanf(f, "%i,%f", &size, &pct ) ; // for now, i wont use size
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
        fscanf(f, "%i,%f", &size, &pct ) ;
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



// 7.500 users out of 100.000 will be for repeated checks
int retry_pool_[7500] ; // round up to optimize for better fit in memory
int next_ ;            // hold the next time when a POP3 check is done



/*
 * Will have a member which will hold the number of mailboxes
   OR a cfg could be passed.
   members:

      retry_pool   7.5%   - 12.50 (repeated)/sec - a check of this mailBox is done between 300 and 600 sec
      random_pool  92.5%  -  4.17 (initial) /sec - a check of this
      --------------------------------------
      user_pool    100%   0 16.65 check/sec
*/


int
Distribute::Pop3::Pop3( config_t* cfg )
{
    // pick random users for the retry_pool
    // and sort the retry_pool array, so we can later
    // perform a bin-search
    for( int i=0; i< 7500 )
    {

        int u = (int)(random()*(1.0*100000)/RAND_MAX) + 1 ;
        // do a sortedInsert here
        retry_pool[i] = u ;
    }
}


int
Distribute::Pop3::~Pop3() {}



/* place a userIdx into user, and return >0 if
 * users messages will be deleted.
 * This is runned 16.65 times per second,
 * Hardcoded 7500 ( which is 7.5% out of 100k users
 */
int
Distribute::Pop3::retry_pool( )
{
    int i = (int)(random()*(1.0*7500)/RAND_MAX) + 1 ;
    return retry_pool_[ i ];
}


// HARDCODED 100000 number of users
int
Distribute::Pop3::random_pool()
{
    //The user picked by this one should NOT be in retry_pool[]
    int i = (int)(random()*(1.0*100000)/RAND_MAX) + 1 ;
    if( !isRetry(i) ) random_pool() ;
    return i ;
}

int
Distribute::Pop3::isRetry( const int p )
{
    // do a bin search of p in retry_pool[]
}

