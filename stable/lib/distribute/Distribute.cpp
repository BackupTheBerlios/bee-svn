#include <cstdlib>
#include <cstdio>
#include "Distribute.h"
#include "structs.h"
//Distribute::Pop3::search


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
    debug( "rcpt_d_.size = %i\n", rcpt_d_.size() );

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
}

Distribute::Smtp::~Smtp() {} ;

// check dis for bug. too tired to look at it
// This should pick if the user is local or remote
int
Distribute::Smtp::rcptTo( rcpt_t a[32], int maxRcpt )
{
	int i=0,c=0,n=0;
    unsigned short xs[3] ;
    float r = (1.0*rand())/RAND_MAX ;
	while ( (r>rcpt_cmf_[i]) && (i<rcpt_d_.size()) )
	{
		i++;
	}
	for( n=i+1;i>=0;--i)
	{
		c = rand() ;
		a[i].idx = (int)(c*(1.0*maxRcpt)/RAND_MAX) + 1 ;
	    r = (1.0*rand())/RAND_MAX ;
        if( r <= 0.473 ) // This distributes the rcptTo, so it should reside in distr
            a[i].local = false ;
	}
	return n ;
}



int
Distribute::Smtp::msgSize()
{
	int i=0;
	float r = (1.0*rand())/RAND_MAX ; 
	while ( (r>msg_cmf_[i]) && (i<msg_d_.size()) )
		i++;
	return msg_d_[i].size*1024 ;
}


int
Distribute::Smtp::mailFrom(int maxRcpt)
{
	int c = rand() ;
	return (int)(c*(1.0*maxRcpt)/RAND_MAX) + 1 ;
}
/* END of DISTRIBUTE::SMTP */





Distribute::Pop3::Pop3( config_t* cfg )
{
	// pick random users for the retry_pool
	// and sort the retry_pool array, so we can later
	// perform a bin-search
    int k =0,c ;
	for( int i=1; i< 100001 ;i+=13) // TODO HARDCODED 100000 nb_users
	{
		int u = i+    (int)((double)rand()*14.0/RAND_MAX);
		retry_pool[k++] = u ;
        debug("\t\tleft=%i\tu=%i\tright=%i", i,u, i+13 ) ;
	}
    for( k=0; k<7500; k++)
        debug("retry user: %i is %i", k, retry_pool[k] );
}


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
	return retry_pool[ i ];
}



// HARDCODED 100000 number of users
int
Distribute::Pop3::random_user()
{
	int i = (int)(random()*(1.0*100000)/RAND_MAX) + 1 ;
	if( isRetry(i) ) random_user() ;	// try again
	return i ;
}



/*
 * search for p inside retry_pool. return the index 
 */
int
Distribute::Pop3::search( const int p, int left, int right )
{
    int mid ;
    if ( right < left ) return -1 ;
	mid = (right-left)/2+left;
    for( int i=1; i<7500; ++i)
    {
        if( p == retry_pool[i] ) return 0 ;
        if( p > retry_pool[i] ) return -1 ;
    }
/*
	if ( p > retry_pool[mid] )
		search( p, mid+1, right ) ;
	else if ( p < retry_pool[mid] )
		search( p, left, mid-1 ) ;
	else
		return mid ;
*/
}



/* HARDODED: 7500 retry users */
int
Distribute::Pop3::isRetry( const int p )
{
	return ( search( p, 0, 7500 ) > -1 ) ? true : false ;
}

/*
	retry_pool   7.5%   - 12.50 (repeated)/sec - a check of this mailBox is done between 300 and 600 sec
	random_pool  92.5%  -  4.17 (initial) /sec - a check of this
	--------------------------------------
	user_pool	100%   0 16.65 check/sec
*/


