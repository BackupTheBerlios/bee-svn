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
/* END of DISTRIBUTE::SMTP */





int
Distribute::Pop3::Pop3( config_t* cfg )
{
	// pick random users for the retry_pool
	// and sort the retry_pool array, so we can later
	// perform a bin-search
	for( int i=0; i< 7500 )
	{
		int u = (int)(random()*(1.0*100000)/RAND_MAX) + 1 ;
		// TODO a sortedInsert here
		retry_pool[i] = u ;
	}
}


int
Distribute::Pop3::~Pop3()
{
}



/* 
 * Return a retry_pool user index.
 * 7500 ( which is 7.5% out of 100k users
 */
int
Distribute::Pop3::retry_user( )
{
	int i = (int)(random()*(1.0*7500)/RAND_MAX) + 1 ;
	return retry_pool_[ i ];
}



// HARDCODED 100000 number of users
int
Distribute::Pop3::random_user()
{
	int i = (int)(random()*(1.0*100000)/RAND_MAX) + 1 ;
	if( isRetry(i) ) random_pool() ;	// try again
	return i ;
}



/*
 * search for p inside retry_pool. return the index 
 */
int
Distribute::Pop3::search( const int p, int left, int right )
{
        int mid ;
        if ( rigth < left ) return -1 ;
	mid = (rigth-left)/2+left;

	if ( p > retry_pool[mid] )
		search( p, mid+1, rigth ) ;
	else if ( p < retry_pool[mid] )
		search( p, left, mid-1 ) ;
	else
		return mid ;
}



/* HARDODED: 7500 retry users */
int
Distribute::Pop3::isRetry( const int p )
{
	return ( search( p, 0, 7500 ) > -1 ) ? true : false ;
}


class Pop3 {
    public:
        Pop3(config_t*) ;
        ~Pop3() ;
        int retry_user() ;
        int random_user();
    private:
        int search( const int p, int left, int rigth ) ;
        int isRetry( const int p ) ;
        int retry_pool[7500] ;
} ;



/*
	retry_pool   7.5%   - 12.50 (repeated)/sec - a check of this mailBox is done between 300 and 600 sec
	random_pool  92.5%  -  4.17 (initial) /sec - a check of this
	--------------------------------------
	user_pool	100%   0 16.65 check/sec
*/


