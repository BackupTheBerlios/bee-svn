#if !defined HTTP_H
#define HTTP_H
#if defined DEBUG
    #include "../../Debug.h"
#endif
#include <socket/Socket.h>
#include <string>

using std::string ;

namespace Http {
    class Protocol : public Socket {
        public:
            struct Url {
                /** URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]. */
                char    host[512] ;
                int     port ;
                char    abs_path[512] ;
                char    query[512] ;
                Url():port(80)
                {
                    memset( host, 0, 512 ) ;
                    memset( abs_path, 0, 512 );
                    memset( query, 0, 512 ) ;
                };
            } ;

            Protocol();
            ~Protocol();
            void open ( const char* host, const int& port );
            void write( const string& message) ;
            void read( ) ;
            void keep_alive( bool k ) { kalive_ = k ; } ; //! set keep-alive
            bool keep_alive( ) { return kalive_ ; } ;     //! get keep-alive value
            void version( int major, int minor ) ;        //! set HTTP version
            void version( int* major, int* minor ) ;      //! get the current version
            void get( const char* url ) ;
            void head( const char* url ) ;
            void post( const char* post_data ) ;
            void put( ) ;
            void del( ) ;
            bool parse_url( const char* url, struct Url* purl ) ;
            void trace( ) ;
            void options( ) ;

        private:
            struct Version { int major ; int minor; } ver_ ;
            bool kalive_ ;
            char host_[512] ; //! can host be more than 512 bytes ?
    };
}

#endif //POP3_H
