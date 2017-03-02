/*
The MIT License (MIT)

Copyright (c) 2017 Leonardo Laguna Ruiz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "BinaryData.h"

#include "LuaVM.h"

LuaVM::LuaVM()
{
   // Creates a lua engine
   L = luaL_newstate();

   // initializes the default libraries
   luaL_openlibs(L);

   // Gets the main script
   int size;
   const char *script = BinaryData::getNamedResource("processor_lua", size);
   // loads the script
   if (luaL_loadstring(L, script))
   {
      printf("Failed to load the main Lua file\n");
   }
   // execute it
   if (lua_pcall(L, 0, LUA_MULTRET, 0))
   {
      printf("Failed to run the main Lua file\n");
   }
}

LuaVM::~LuaVM()
{
   lua_close(L);
}

void LuaVM::setInputBuffers(const float *left, const float *right)
{
   lua_getglobal(L, "setInputBuffers");
   lua_pushlightuserdata(L, (void *)left);
   lua_pushlightuserdata(L, (void *)right);
   if (lua_pcall(L, 2, 0, 0) != 0)
      printf("error running function 'setInputBuffers': %s", lua_tostring(L, -1));
}

void LuaVM::setOutputBuffers(float *left, float *right)
{
   lua_getglobal(L, "setOutputBuffers");
   lua_pushlightuserdata(L, (void *)left);
   lua_pushlightuserdata(L, (void *)right);
   if (lua_pcall(L, 2, 0, 0) != 0)
      printf("error running function 'setOutputBuffers': %s", lua_tostring(L, -1));
}

void LuaVM::process(int samples)
{
   lua_getglobal(L, "process");
   lua_pushnumber(L, samples);
   if (lua_pcall(L, 1, 0, 0) != 0)
      printf("error running function 'process': %s", lua_tostring(L, -1));
}

var LuaVM::loadFiles(var code)
{
   // call the lua function 'loadCode'
   lua_getglobal(L, "loadCode");
   lua_pushstring(L, code.toString().toRawUTF8());
   if (lua_pcall(L, 1, 1, 0) != 0)
      printf("error running function 'loadCode': %s", lua_tostring(L, -1));
   bool result = lua_tonumber(L, -1) ? true : false;
   return var(result);
}
