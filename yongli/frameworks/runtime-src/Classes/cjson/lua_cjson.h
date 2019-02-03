
#ifndef __LUA_CJSON_H_
#define __LUA_CJSON_H_

#if __cplusplus
extern "C" {
#endif

#include "lua.h"
#include <lauxlib.h>
#include <lualib.h>
#define USE_INTERNAL_FPCONV

int luaopen_cjson(lua_State *l);

#if __cplusplus
}
#endif

#endif // __LUA_CJSON_H_
