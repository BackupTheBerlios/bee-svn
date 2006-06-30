extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}




#include "wrapper.h"

class LuaWrapper
{
      public:
            static const char className[];
            static const luaL_reg methods[];

      public:
            static Wrapper* checkWrapper( lua_State* L, int narg);
            static int create_Wrapper( lua_State *L);
            static int send( lua_State *L);
            static int recv( lua_State *L);
            static int findLine( lua_State *L);
            static int lmatch( lua_State *L);
            static int smatch( lua_State *L);
            static int mmatch( lua_State *L);
            static int mwc( lua_State *L);
            static int wc( lua_State *L);
            static int cut( lua_State *L);
            static int size( lua_State *L);
            static int replicate( lua_State *L);
            static int b64( lua_State *L);
            static int close( lua_State *L);
            static int timeout( lua_State *L);
            static int gc_Wrapper( lua_State *L);
            static void Register( lua_State* L);
};
