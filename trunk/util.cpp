/*!   \brief Functions used for parsing balu.conf, creating threads.\n
 */
#include "util.h"

vector<Connection_t> conVector;
extern vector< ThreadInfo_t> scripts;




/*!   Run a LUA interpreter.\n
 *    \param[in]  argv        Lua script, stored in memory
 *    \return     void*/
void*
luaThread( void* argv)
{
      lua_State* L =lua_open();
      luaopen_base( L);
      luaopen_table( L);
      luaopen_string( L);
      LuaWrapper::Register( L);
      LuaConfig::Register( L);

      int retval =lua_dostring( L, (char*) argv);

      if( retval == LUA_ERRFILE)
      {
            cerr <<"mmk: cannot load script file" <<argv <<endl;;
      }
      else
            if( retval == LUA_ERRMEM)
            {
                  cerr <<"mmk: interpreter out-of-memory error\n" <<endl;
            }
            else
                  if(retval == LUA_ERRMEM)
                  {
                        cerr <<"mmk: interpreter error-handling error\n" <<endl;
                  }
      if( retval != 0)
      {
            exit(EXIT_FAILURE);
      }
      lua_close( L);
      return argv;
}//   luaThread




/*!   parse balu.conf
 *    \param[in] filename Is the config filename
 *    \return cfg_t
 */
cfg_t*
parseConf(string filename)
{
      void* p();
      cfg_opt_t stored_opts[] ={
            CFG_INT( "mails", 0, CFGF_NONE),
            CFG_STR( "date", "", CFGF_NONE),
            CFG_END()
      };

      cfg_opt_t connection_opts[] ={
            CFG_STR( "host", 0, CFGF_NODEFAULT),
            CFG_INT( "port", 25, CFGF_NONE),
            CFG_STR( "user", "user1", CFGF_NONE),
            CFG_STR( "pass", "user1", CFGF_NONE),
            CFG_STR( "domain", "localdomain", CFGF_NONE),
            CFG_SEC( "stored", stored_opts, CFGF_MULTI | CFGF_TITLE),
            CFG_END()
      };

      cfg_opt_t opts[] =
      {
            CFG_SEC( "connection", connection_opts, CFGF_MULTI | CFGF_TITLE),
            CFG_FUNC( "include-file", cfg_include),
            CFG_END()
      };

      cfg_t* cfg =cfg_init( opts, CFGF_NONE);

      switch( cfg_parse( cfg, filename.c_str()))
      {
            case CFG_FILE_ERROR:
                  printf( "ERR<< Config(file not found)(%s)\n", filename.c_str());
                  exit( 1);
            case CFG_SUCCESS:
                  break;
            case CFG_PARSE_ERROR:
                  return 0;
      }
      return cfg;
}//   parseConf




/*!   parse balu.conf and fill in conVector
 */
void
readConfig( void) {
      int i,j;
      cfg_t* cfg =parseConf( "balu.conf");

      if( cfg)
      {
            for( i =0; i <cfg_size( cfg, "connection"); i++)
            {
                  cfg_t *connection =cfg_getnsec( cfg, "connection", i);
                  Connection_t tmpCon;
                  Stored_t tmpStored;
                  for( j =0; j <cfg_size( connection, "stored"); j++)
                  {
                        cfg_t*  stored =cfg_getnsec( connection, "stored", j);
                        tmpStored.date_s =string( cfg_getstr( stored, "date"));
                        tmpStored.mails_ui =cfg_getint( stored, "mails");
                        tmpCon.storedVector.push_back( tmpStored);
                  }
                  tmpCon.user_s   =string( cfg_getstr( connection, "user"));
                  tmpCon.pass_s   =string( cfg_getstr( connection, "pass"));
                  tmpCon.host_s   =string( cfg_getstr( connection, "host"));
                  tmpCon.domain_s =string( cfg_getstr( connection, "domain"));
                  tmpCon.port_ui   =        cfg_getint( connection, "port");
                  //  Save connection-info in a vector
                  conVector.push_back(tmpCon);
            }
            cfg_free(cfg);
      }
      else
      {
            cout <<"Config file ERR<<( " <<"balu.conf" <<")" <<endl;
      }
}//   readConfig




/*!
\todo Parse scripts data structure do a "for loop" for each record.
\todo balu file.lua run file.lua on 1 thread
\todo balu -t 2 file_1.lua -t 3 file_2.lua run file_1.lua on 2 threads...*/
void
doThreads( )
{
      int i,j;
      pthread_t   tid[1024];
      pthread_attr_t tattr;
      pthread_attr_init( &tattr);

      //cout <<"size:" <<scripts.size() <<endl;


      for( j =0; j <scripts.size(); ++j)
      {
            //----------Thread create------------
            for( i =0; i <scripts[ j].numThreads; ++i)
            {
                  if( pthread_create( &tid[i], &tattr, luaThread, (void*)scripts[ j].script.c_str()) < 0)
                  {
                        cerr <<"Failed to create thread "<< i <<endl;
                  }
                  else
                  {
                        //cout <<"Created thread " <<i;
                  }
            }
            // Wait for the threads to terminate and cleanup
            for( i =0; i < scripts[ j].numThreads; ++i)
            {
                  pthread_join(tid[i], NULL);
            }
      }
}


