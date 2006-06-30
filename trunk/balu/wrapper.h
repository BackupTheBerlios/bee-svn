#ifndef TCP_H
#define TCP_H

#include <string>
#include <vector>
#include "socket.h"
#include "debug.h"

using namespace std;
class Wrapper
{
      private:
            Socket m_sock;
      public:
            string response;


      public:
            Wrapper( string hostname, int port);
            string replicate( string text, int times);
            void   send( string command) ;
            void   recv(string printResponse);
            void   tokenize( const string& str, vector<string>& tokens,
                             const string& delimiters);
            int    findLine( string textToFind);
            string lmatch( int lineNo, string expect_regex);
            string smatch( string expect_regex);
            void   mmatch( string expect_regex, string until_regex);
            string cut( string textToCut, int cutField);
            int    mwc( unsigned int line, unsigned int fields);
            int    wc ( unsigned int line);
            void   timeout( const long int seconds);
            void   close( void);
            string b64( const string& text);
            unsigned int  size( string);
      private:
            void   info( const char* prefix, const char* format, ...);
            void   error( const char* prefix, const char* where, const char *format, ...);
};
#endif
