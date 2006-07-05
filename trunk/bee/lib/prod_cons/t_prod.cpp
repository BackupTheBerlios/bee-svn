#include "Producer.hpp"

int run =1 ;
class Job {
    public:
    Job()
    {
    };
    int userIdx ;
};

class SmtpTest {
    public:
    int userIdx ;
    SmtpTest( )
    {
        userIdx = 10 ;
    };
};


/*!
 * This is an example of how a Smtp producer would work.<br>
 * Each type of producer MUST override this method.
 **/
template<class T, class Arg>
T*
Producer<T,Arg>::produce( Arg* a )
{
    printf("called produce\n" ) ;
        T*      t ;    //! Temporary job
        t = new T ;

        // Fill in the data fields.
        t->userIdx = a->userIdx ;
#if 0
        t->userIdx   = pickUserIdx_( arg->domains[0].usrInDomain ) ;
        t->domainIdx = pickDomIdx_ ( arg->domains[0].count ) ;
        t->fileIdx   = pickFileIdx_(  ) ;
#endif
        return t ;
}//* Producer::produce

int
main( int argc, char* argv[] )
{
    SmtpTest smtpTest ; //! smtpTest is used as a argument, which Might be used
                        //! inside produce().
    Producer<Job, SmtpTest> p(&smtpTest) ;
    p.execute( &p ) ;
    return 0 ;
}
    //sscanf( argv[1], "%d", &no ) ;