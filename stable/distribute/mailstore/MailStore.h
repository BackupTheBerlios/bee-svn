#include <vector>
#include <pthread.h>

using namespace std;




class MailStore {
    public:
        MailStore(const char* fname) ;
        ~MailStore(){} ;
        struct dist_t {
            int   msg;
            float pct ;
            float step ;
        } ;
        struct targ { 
            int mbi ;  // current mailbox
            int mbf ;  // mailboxes filled
            int end ;
            pthread_mutex_t* mtx ;
            MailStore* p ;
            int delay ;
        } arg ;
        static void* fillMbox( void* ) ;
        int init( const char* host, const int port, const int maxMbox, const int threads) ;
    private:
        void sendMails(const int userIdx, const int quantity ) ;
        vector<struct dist_t> md ;
    /* Message distribution over mailbox */
} ;

