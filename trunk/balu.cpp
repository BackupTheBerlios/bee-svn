#include <iostream>
#include <pthread.h>
#include "util.h"

using namespace std;
vector< ThreadInfo_t> scripts;



const char
LuaWrapper::className[]  ="Tcp";

const luaL_reg
LuaWrapper::methods[]  ={
      method( LuaWrapper, send),
      method( LuaWrapper, recv),
      method( LuaWrapper, b64),
      method( LuaWrapper, findLine),
      method( LuaWrapper, cut),
      method( LuaWrapper, lmatch),
      method( LuaWrapper, smatch),
      method( LuaWrapper, mmatch),
      method( LuaWrapper, mwc),
      method( LuaWrapper, wc),
      method( LuaWrapper, size),
      method( LuaWrapper, close),
      method( LuaWrapper, timeout),
      method( LuaWrapper, replicate),
      { 0, 0}
};

const char
LuaConfig::className[]  ="Config";

const luaL_reg
LuaConfig::methods[]  ={
      method( LuaConfig, host),
      method( LuaConfig, domain),
      method( LuaConfig, port),
      method( LuaConfig, user),
      method( LuaConfig, pass),
      method( LuaConfig, mails),
      method( LuaConfig, date),
      { 0, 0}
};



/*!   Load a LUA script in memory
 *    \param[in]  fileName is the name of the LUA script
 *    \return     A string holding the script*/
string
loadScript(string fileName)
{
      RING;
      FILE* f =fopen( fileName.c_str(), "r");
      if( NULL ==f)
      {
            cerr <<"Can't open " <<fileName <<endl;
            exit(1);
      }

      char s[1024];
      string script;
      while( ! feof( f))
      {
            fgets(s, 1024, f);
            script +=s;
      }
      script +='\0';
      return script;
}



/*!   Parse the CLI options
 *    \param[in]  argc  Is the main() argc parameter
 *    \param[in]  argv  Is the main() argv parameter
 *    \todo       Check if after -t folows an uint
      \todo       Check if after -t n folows a script name*/
void
parseOptions( int argc, char* argv[])
{
      ThreadInfo_t tmpScript;
      RING;
      int i=1;

//      cout <<"ARGC" <<argc <<endl;


      while( i <argc)
      {
            //cout <<"i" <<i <<endl;

            if( ! strcmp( argv[ i], "-t"))
            {
                  string fileName="";
                  int numThreads=0;
                  // Get the number of threads.
                  ++i;
                  numThreads =atoi( argv[ i]);
                  cout <<"numThreads: " <<numThreads <<endl;
                  tmpScript.numThreads =numThreads;

                  // Get the file name.
                  ++i;
                  fileName =argv[ i];
                  cout <<"fileName: "<<fileName<<endl;
                  //\todo the filename should be tested
                  tmpScript.script =( loadScript( fileName));
                  scripts.push_back( tmpScript);
                  continue;
            }
            //cout <<"Threads: "  <<endl;
            tmpScript.numThreads =1;
            tmpScript.script =loadScript( argv[ i]);
            scripts.push_back( tmpScript);
            ++i;
      }
}




/*!
*/
      Info info_m;
int
main( int argc, char* argv[])
{
      RING;
      info_m.runedScript =argv[1];
      readConfig();
      parseOptions( argc, argv);
      doThreads();
      return 0;
}//   main




