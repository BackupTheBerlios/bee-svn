
class RateGenerator {
    public:
        RateGenerator( ); 
        RateGenerator( int seed, double mean ); 
        ~Rate() ;
        double uniform( double upper, double lower ) ;
        double exponential( ) ;
        mean( double m ) { mean_ = m ; } 
    private:
        u_short xsubi_[3] ;
        double mean_ ;

};

RateGenerator::RateGenerator( int seed, double m )
{
  xsubi_[0] = 0x1234 ^ seed;
  xsubi_[1] = 0x5678 ^ (seed << 8);
  xsubi_[2] = 0x9abc ^ ~seed;
  mean_ = m ; 
}



double
RateGenerator::uniform( double upper, double lower ) 
{
  return lower + (upper - lower)*erand48( xsubi_ ) ;
}

double
RateGenerator::exponential() 
{
  return -mean_*log( 1.0 - erand48(xsubi_) ) ;
}
