#include "luaWrapper.h"

using namespace std;

Wrapper*
LuaWrapper::checkWrapper( lua_State* L, int narg)
{
      luaL_checktype( L, narg, LUA_TUSERDATA);
      void* ud =luaL_checkudata( L, narg, className);
      if( ! ud)
            luaL_typerror( L, narg, className);
      // unbox pointer
      return *( Wrapper**) ud;
}




int
LuaWrapper::create_Wrapper( lua_State *L)
{
      string host =luaL_check_string( L, 1);
      int port =( int)luaL_checknumber( L, 2);
      Wrapper* a =new Wrapper( host, port);
      //Box Pointer?
      lua_boxpointer( L, a);
      //get meta table?
      luaL_getmetatable( L, className);
      // set metatable?
      lua_setmetatable( L, -2);
      return 1;
}




int
LuaWrapper::send( lua_State *L)
{
      Wrapper *a =checkWrapper( L, 1);
      string command =luaL_check_string( L, 2);
      a->send( command);
      return 0;
}



int
LuaWrapper::close( lua_State *L)
{
      Wrapper *a =checkWrapper( L, 1);
      a->close( );
      return 0;
}




int
LuaWrapper::recv( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string printResponse =luaL_check_string( L, 2);
      a->recv( printResponse);
      return 0;
}




int
LuaWrapper::timeout( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      long int seconds =(long int)luaL_checknumber( L, 2);
      a->timeout( seconds);
      return 0;
}




int
LuaWrapper::findLine( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string textToFind =luaL_check_string( L, 2);
      int line =a->findLine( textToFind);
      lua_pushnumber( L, line);
      return 1;
}




int
LuaWrapper::lmatch( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      int lineNo =( int)luaL_checknumber( L,2 );
      string expect_regex =luaL_check_string( L, 3);
      string found =a->lmatch( lineNo, expect_regex);
      lua_pushstring( L, found.c_str());
      return 1;
}




int
LuaWrapper::smatch( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string textToFind =luaL_check_string( L, 2);
      string found =a->smatch( textToFind);
      lua_pushstring( L, found.c_str());
      return 1;
}




int
LuaWrapper::mmatch( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string expect_regex =luaL_check_string( L, 2);
      string until_regex  =luaL_check_string( L, 3);
      a->mmatch( expect_regex, until_regex);
      return 0;
}




int
LuaWrapper::mwc( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      unsigned int line =( unsigned int)luaL_checknumber( L, 2);
      unsigned int fields =( unsigned int)luaL_checknumber( L, 3);
      a->mwc( line, fields);
      return 0;
}




int
LuaWrapper::wc( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      unsigned int line  =( unsigned int)luaL_checknumber( L, 2);
      int fields =a->wc( line);
      lua_pushnumber( L, fields);
      return 1;
}




int
LuaWrapper::cut( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string text =luaL_check_string( L, 2);
      int field =( int)luaL_checknumber( L, 3);
      string ret =a->cut( text, field);
      lua_pushstring( L, ret.c_str());
      return 1;
}




int
LuaWrapper::size( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string text =luaL_check_string( L, 2);
      unsigned int ret =a->size( text);
      lua_pushnumber( L, ret);
      return 1;
}




int
LuaWrapper::replicate( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string text =luaL_check_string( L, 2);
      int times =( int)luaL_checknumber( L, 3);
      string ret =a->replicate( text, times);
      lua_pushstring( L, ret.c_str());
      return 1;
}




int
LuaWrapper::b64( lua_State *L)
{
      Wrapper* a =checkWrapper( L, 1);
      string text =luaL_check_string( L, 2);
      string b64 =a->b64( text);
      lua_pushstring( L, b64.c_str());
      return 1;
}




int
LuaWrapper::gc_Wrapper( lua_State *L)
{
      Wrapper* a =( Wrapper*)lua_unboxpointer( L, 1);
      delete a;
      return 0;
}




void
LuaWrapper::Register( lua_State* L)
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
      lua_pushcfunction( L, gc_Wrapper);
      lua_settable( L, metatable);

      // drop metatable
      lua_pop( L, 1);

// fill methodtable
      luaL_openlib( L, 0, methods, 0);
      // drop methodtable
      lua_pop( L, 1);

      lua_register( L, className, create_Wrapper);
}
