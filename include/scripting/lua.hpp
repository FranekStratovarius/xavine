#ifndef LUA_HPP
#define LUA_HPP

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

lua_State* load_lua();

void close_lua(lua_State* luastate);

void run_lua(lua_State* luastate);

/*

action = GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE

*/
void lua_key_pressed(lua_State* luastate,const char* key,int action);

#endif
