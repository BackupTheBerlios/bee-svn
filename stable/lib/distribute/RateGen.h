#if !defined _RATE_GEN_H_
#define _RATE_GEN_H_

class RateGen {
    public:
        RateGen( ) ; 
        RateGen( int seed, double freq ) ; 
        ~RateGen() {};
        double  uniform( double upper, double lower ) ;
        double  exponential( ) ;
        
        double  mean() { return mean_ ; } ;
        void    mean(double m) { mean_ = m ; } ;
        double  rate() { return freq_ ; } ;
        void    rate(double f) { freq_ = f ; } ;
    private:
        double  freq_ ;          // rate parameter  ( frequency )
        double  mean_ ;          // 1/freq          (wave length)
        unsigned short xsubi_[3] ;

};

#endif //_RATE_GEN_H_
