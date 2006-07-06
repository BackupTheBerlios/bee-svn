extern "C"
{
#include <lauxlib.h>
}




#include "config.h"

class LuaConfig
{
      public:
            static const char className[];
            static const luaL_reg methods[];

      public:
            static Config* checkConfig( lua_State* L, int narg);
            static int create_Config( lua_State *L);
            static int host( lua_State *L);
            static int port( lua_State *L);
            static int user( lua_State *L);
            static int domain( lua_State *L);
            static int pass( lua_State *L);
            static int mails( lua_State *L);
            static int date( lua_State *L);
            static int gc_Config( lua_State *L);
            static void Register( lua_State* L);
      // LuaConfig
};
