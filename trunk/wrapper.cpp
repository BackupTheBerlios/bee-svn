/*!   \class Wrapper Wrapper.h "Wrapper.h"
 *    \brief Implements the functions used in lua scripts.\n
 *    This is done by calling functions from the Socket class,
 *    for the send, reveive and close functions\n
 *    or by calling functions from libpcre, for matching functions.\n
 *
 *    \see Socket */
#include <iostream>
#include <sstream>

//#include <boost/tokenizer.hpp>
#include "wrapper.h"
#include "regex.h"
#include "base64.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "debug.h"


using namespace std;
extern Info info_m;
//using namespace boost;

/*!   \todo function that send a mail stored in a .eml file*/

/*!   Create a socket and connects it.\n
 *    \see        Socket::create
 *    \param[in]  hostName    the host name.
 *    \param[in]  port:       port number.
 *    \return     void*/
Wrapper::Wrapper( string hostName, int port){
      RING;
      try
      {
            m_sock.create( AF_INET, SOCK_STREAM, 0);
            m_sock.connect( hostName.c_str(), port);
      }
      catch( char* exception)
      {
            cout <<"Exception: " <<exception <<endl;
            info_m.error( "cucu" ,"Wrapper", exception, "");
      }

      string banner;
      m_sock.recv( banner);
      info_m.info( "Wrapper", "banner:%s", banner.c_str());
}//   Wrapper::Wrapper




/*!   Send a string to socket.\n
 *    \param[in]  command string that will be sent through socket.
 *    \return     void.*/
void
Wrapper::send( string command)
{
      RING;
      try
      {
            info_m.info( "SEND", "%s",  command.c_str());
            m_sock.send( command.c_str());
      }
      catch( char* exception)
      {
            cout <<"Exception: " <<exception <<endl;
            info_m.error( "SEND", "Can't send", "");
      }
}//   Wrapper::send




/*!   Closes connection.\n
 *    \return     void.*/
void
Wrapper::close( void)
{
      RING;
      //bool closed =m_sock.close();
      bool closed=true;
      if( ! closed)
      {
            info_m.error( "socket","closing socket", "Can't close socket");
      }
      else
      {
            info_m.info( "socket", "closed");
      }
}




/*!   Set the receive timeout.\n
 *    \param[in]  seconds Number of seconds to wait.
 *    \return     void.*/
void
Wrapper::timeout( const long int seconds)
{
      RING;
      m_sock.timeout =seconds;
}//   Wrapper::timeout




/*!   Receive a string from socket.\n
 *    \param[in]  printResponse decide if response is printed to cout.
 *    \return     void.*/
void
Wrapper::recv( string printResponse)
{
      RING;
      m_sock.recv( response);
      if( printResponse == "yes")
      {
            info_m.info( "RECV", "\n%s", response.c_str());
      }
      return ;
}//   Wrapper::recv




/*!   Finds a line in the server's response.\n
 *    \param[in]  textToFind Is the text that will be searched.
 *    \retval     n Representing the line number, if textToFind was found.
 *    \retval     -1 If error ocured, or textToFind wasn't found*/
int
Wrapper::findLine( string textToFind)
{
      RING;
      vector<string> tokens;
      string delimiters ="\r\n", out;
      tokenize( response, tokens, delimiters);

      try
      {
            RegEx Expect( textToFind.c_str());
            for( int i =0; i < tokens.size(); i++)
            {
                  string line =tokens[i];
                  if( Expect.Search( line.c_str()))
                  {
                        info_m.info( "findline" , line.c_str());
                        return i;
                  }
            }
      }
      catch( const char* Error)
      {
            info_m.error("findLine", Error, textToFind.c_str());
            return -1;
      }
}//   Wrapper:findLine




/*!   Matches a certain line from server's response with a regex.\n
 *    \param[in]  lineNo Line that will be matched.
 *    \param[in]  expect_regex Regular epression after wich the matching is done.
 *    \retval     string If lineNo matches expect_regex, returns line "lineNo".
 *    \retval     empty_string If line "lineNo" doesn't match expect_regex .*/
string
Wrapper::lmatch(int lineNo, string expect_regex)
{
      RING;
      if( lineNo == -1)
      {
            info_m.error( "lmatch", "Negative value", "line:%i", lineNo);
            exit(1);
      }

      vector<string> tokens;
      string delimiters ="\n", out;

      tokenize( response, tokens, delimiters);
//      for( int i =0; i<tokens.size(); i++)
  //          cout << tokens[ i] <<endl;

      if( lineNo+1 > tokens.size())
      {
            info_m.error("lmatch", "Out of range", "line:%i", lineNo);
            return "";
      }

      try
      {
            RegEx Expect( expect_regex.c_str());
            string line =tokens[ lineNo];

            if( Expect.Search( line.c_str()) )
            {
                  info_m.info("lmatch", line.c_str());
                  if( tokens[ lineNo].size())
                        return tokens[ lineNo];
            }
            else
            {
                  info_m.error( "lmatch", "Can't find regex", "%s", expect_regex.c_str());
                  return "";
            }

      }
      catch( char* Error)
      {
            info_m.error( "lmatch" ,Error ,"%s", expect_regex.c_str());
            exit(1);
      }
}//   Wrapper::lmatch



//--here--
/*!  Search a string in a singleline response.
 *   \param[in] expect_regex Is the regular expression after wich the match is done.
 *   \retval server_response When the response matches the regular expression
 *   \retval empty_string When response doesn't match the regular expression*/
string
Wrapper::smatch( string expect_regex)
{
      RING;
      string tmpResponse =response;
      try
      {
            RegEx Until( expect_regex.c_str());
            if( Until.Search( tmpResponse.c_str()))
            {
                  info_m.info( "smatch", "%s", tmpResponse.c_str());
                  Until;
                  return tmpResponse;
            }
            else
            {
                  info_m.error( "smatch", "can't match regex", "%s", expect_regex.c_str());
                  info_m.error( "smatch", "can't match line", "%s", response.c_str());
                  return "";
            }
      }
      catch( const char* Error)
      {
            info_m.error( "smatch", Error, "%s", expect_regex.c_str());
            exit(1);
      }
}//   Wrapper::smatch




/*!   Test a multiline response, on a certain regex.\n
 *    If the string matches the regexp, an OK message is sent to cout.\n
 *    otherwise an ERR message is sent to cerr
 *    \param[in] expect_regex Is the regular expression after wich each line is matched.
 *    \param[in] until_regex Is the regular expression of the last response line
 *    \return void
 */
void
Wrapper::mmatch( string expect_regex, string until_regex)
{
      RING;
      try
      {
            vector<string> tokens;
            string delimiters ="\r\n", out;
            tokenize( response, tokens, delimiters);
            RegEx Expect( expect_regex.c_str());
            RegEx Until( until_regex.c_str());
            string line;
            for( int i =0; i < tokens.size(); i++)
            {
                  line =tokens[i];

                  if( Until.Search( line.c_str()))
                  {
                        info_m.info( "ummatch", "%s", line.c_str());
                        break;
                  }

                  if( Expect.Search( line.c_str()))
                  {
                        info_m.info( "emmatch", "%s", line.c_str());
                  }
                  else
                  {
                        info_m.error( "mmatch", "can't match regex", "%s", expect_regex.c_str());
                        info_m.error( "mmatch", "can't match line", "%s", line.c_str());
                  }
            }
      }
      catch( const char* Error)
      {
            info_m.error( "mmatch", Error, "%s", until_regex.c_str());
            exit(1);
      }
}//   Wrapper::mmatch




/*!   Match word count in a line.\n
 *    If the line has <i>fields</i> words, an OK  message is sent to cout\n
 *    else an ERR message is sent to cerr
 *    \param[in] line Is the line of wich words will be counted
 *    \param[in] fields Is the expected number of words
 */
int
Wrapper::mwc( unsigned int line, unsigned int fields)
{
      RING;
      if( ( line < 0) || ( fields == 0))
      {
            info_m.error( "mwc", "negative line or field", "%i != %i", line, fields);
            return 0;
      }

      if( line == 0)
      {
            vector<string> words;
            string delimiters =" ", out;
            tokenize( response, words, delimiters);
            int size =words.size();
            if( size != fields)
            {
                  info_m.error( "mwc", "fields don't match", "%i != %i", fields, size);
                  return 0;
            }
      }
      else
      {
            vector<string> lines;
            string delimiters ="\r\n", out;
            tokenize( response, lines, delimiters);
            if( lines.size() < line)
            {
                  info_m.error( "mwc", "line out of range", "%i", line);
                  return 0;
            }
            vector<string> words;
            delimiters =" ";
            tokenize( lines[ line-1], words, delimiters);
            int size =words.size();
            if( size != fields)
            {
                  info_m.error( "mwc", "fields don't match", "r%i != %ie", size, fields);
                  return 0;
            }
            info_m.info( "mwc", "fields match: r%i == %ie", size, fields );
      }
}//   Wrapper::mwc




/*!   Returns the number of words, in a certain line.
 *    \param[in] line Is the line of wich words will be counted.*/
int
Wrapper::wc( unsigned int line)
{
      RING;
      if( line <0)
      {
            info_m.error( "wc", "negative line number", "%i", line);
            exit( 1);
      }
      if( line == 0)
      {
            vector<string> tokens;
            string delimiters =" ", out;
            tokenize( response, tokens, delimiters);
            int size =tokens.size();
            return size+1;
      }
      else
      {

            //   tokenize( response) after \r\n
            vector<string> lines;
            string delimiters ="\r\n", out;
            tokenize( response, lines, delimiters);
            if( lines.size() < line)
            {
                  info_m.error( "wc", "line out of range", "%i", line);
                  return 1;
            }
            //   words =tokenize( line) after space
            vector<string> words;
            delimiters =" ";
            tokenize( lines[ line-1], words, delimiters);
            return words.size();
      }
}//   Wrapper::wc




/*!   Cut a certain field from a string
 *    \param[in] textToCut Is the string on wich the operations are done
 *    \param[in] cutField Is the field that will be returned
 *    \return The <i>cutField</i> field as a string
 *    \todo should exclude all the [](){} "'*/
string
Wrapper::cut( string textToCut, int cutField)
{
      RING;
      vector<string> tokens;
      string delimiters =" ", out;
      tokenize( textToCut, tokens, delimiters);
//   the size start from 1, cutField from 0
      if( tokens.size() < ( cutField+1))
      {
            info_m.error( "cut", "out of range cut" , "%i", cutField);
            exit(1);
      }
      return tokens[ cutField];
}//   Wrapper::cut




/*!   Compute the size of a string.
 *    \param[in] text Is the string on which to operate
 *    \return The size of <i>text</i>*/
unsigned int
Wrapper::size( string text)
{
      RING;
      return text.size();
}//   Wrapper::size




/*!   Split a string, after certain delimiters.
 *    \param[in] str Is the string that will be splited.
 *    \param[in] delimiters Is a string after which the splitting is done
 *    \param[out] tokens Is a vector containing the splits
 *    \return void */
void
Wrapper::tokenize( const string& str, vector<string>& tokens, const string& delimiters)
{
      RING;
      string::size_type lastPos =str.find_first_not_of(  delimiters, 0 );
      string::size_type pos     =str.find_first_of(  delimiters, lastPos );

      while( string::npos != pos || string::npos != lastPos)
      {
            tokens.push_back( str.substr( lastPos, pos-lastPos));
            lastPos =str.find_first_not_of( delimiters, pos);
            pos =str.find_first_of( delimiters, lastPos);
      }
}//   Wrapper::tokenize




/*!   Replicate a string
 *    \param[in] text Is the text to be replicated.
 *    \param[in] n Is the number of replicas.
 *    \return text X n */
string
Wrapper::replicate( string text , int n)
{
      RING;
      if( text.empty())
      {
            info_m.error( "replicate" ,"empty string", "");
            return "";
      }
      unsigned int size =text.size();
      if( n <=0)
      {
            info_m.error( "replicate", "out of range", "%i", n);
            return "";
      }

      char* finalStr =( char*)malloc(n*size +1);
      if( finalStr ==NULL)
      {
            info_m.error("replicate", "can't allocate", "%i", errno);
            return "";
      }

      for( int i =0; i <n; i++)
      {
            memcpy( ( finalStr +i*size), text.c_str(), size);
      }
      finalStr[n*size+1] ='\0';
      return string( finalStr);
}//   Wrapper::replicate




/*!   Encode a string using BASE64 algorithm
 *    \param[in] text Is the text to be ecoded in base64.
 *    \return base64_string The encoded string */
string
Wrapper::b64( const string& text)
{
      RING;
      base64<char> encoder;
      int _State =0;

      stringstream os;
      os << text;
      string didi;

      istreambuf_iterator<char> _From(os.rdbuf());
      istreambuf_iterator<char> _To(0);

      stringstream ostr;
      ostreambuf_iterator<char> _Out(ostr);
      encoder.put(_From, _To, _Out, _State, base64<>::crlf());
      ostr >> didi;
      return didi;
}//   Wrapper::b64
