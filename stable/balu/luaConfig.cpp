#include "luaConfig.h"

using namespace std;

Config*
LuaConfig::checkConfig( lua_State* L, int narg)
{
      luaL_checktype( L, narg, LUA_TUSERDATA);
      void* ud =luaL_checkudata( L, narg, className);
      if( ! ud)
      {
            luaL_typerror( L, narg, className);
      }
      // unbox pointer
      return *( Config**) ud;
}




int
LuaConfig::create_Config( lua_State *L)
{
      string confFile =luaL_check_string( L, 1);
      Config* a =new Config( 1);
      lua_boxpointer( L, a);
      luaL_getmetatable( L, className);
      lua_setmetatable( L, -2);
      return 1;
}




int
LuaConfig::host( lua_State *L)
{
      Config *a =checkConfig( L, 1);
      int con =( int)luaL_checknumber( L, 2);
      string hostname =a->host( con);
      lua_pushstring( L, hostname.c_str());
      return 1;
}




int
LuaConfig::port( lua_State *L)
{
      Config *a =checkConfig( L, 1);
      int con =( int)luaL_checknumber( L, 2);
      int port =a->port( con);
      lua_pushnumber( L, port);
      return 1;
}




int
LuaConfig::user( lua_State *L)
{
      Config *a =checkConfig( L, 1);
      int con =( int)luaL_checknumber( L, 2);
      string username =a->user( con);
      lua_pushstring( L, username.c_str());
      return 1;
}




int
LuaConfig::domain( lua_State *L)
{
      Config *a  =checkConfig( L, 1);
      int connection =( int)luaL_checknumber( L, 2);
      string domain  =a->domain( connection);
      lua_pushstring( L, domain.c_str());
      return 1;
}




int
LuaConfig::pass( lua_State *L)
{
      Config *a =checkConfig( L, 1);
      int con =( int)luaL_checknumber( L, 2);
      string password =a->pass( con);
      lua_pushstring( L, password.c_str());
      return 1;
}




int
LuaConfig::date( lua_State *L)
{
      Config *a =checkConfig( L, 1);
      int connec =( int)luaL_checknumber( L, 2);
      int stored =( int)luaL_checknumber( L, 3);
      string date =a->date( connec, stored);
      lua_pushstring( L, date.c_str());
      return 1;
}




int
LuaConfig::mails( lua_State *L)
{
      Config *a =checkConfig( L, 1);
//   get the parameter from the stack
      int connec =( int)luaL_checknumber( L, 2);
      int stored =( int)luaL_checknumber( L, 3);
      unsigned int mails =a->mails( connec, stored);
//   push the parameters onto stack
      lua_pushnumber( L, mails);
      return 1;
}




int
LuaConfig::gc_Config( lua_State *L)
{
      Config* a =( Config*)lua_unboxpointer( L, 1);
      delete a;
      return 0;
}




void
LuaConfig::Register( lua_State* L)
{
      lua_newtable( L);
      int methodtable =lua_gettop( L);
      luaL_newmetatable( L, className );
      int metatable   =lua_gettop( L);

      lua_pushliteral( L, "__metatable");
      lua_pushvalue( L, methodtable);
      // hide metatable from Lua getmetatable()
      lua_settable( L, metatable);

      lua_pushliteral( L, "__index");
      lua_pushvalue( L, methodtable);
      lua_settable( L, metatable);

      lua_pushliteral( L, "__gc");
      lua_pushcfunction( L, gc_Config);
      lua_settable( L, metatable);

      // drop metatable
      lua_pop( L, 1);

// fill methodtable
      luaL_openlib( L, 0, methods, 0);
      // drop methodtable
      lua_pop( L, 1);

      lua_register( L, className, create_Config);
}
