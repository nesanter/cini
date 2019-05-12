/* Copyright © 2019 Noah Santer <personal@mail.mossy-tech.com>
 *
 * This file is part of cini.
 *
 * cini is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cini is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cini.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "table.h"

#include <stdio.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>

struct table * table_alloc()
{
    lua_State * L = luaL_newstate();
    return (struct table *)(void *)L;
}

void ** table_get(
        struct table * table,
        const char * key,
        size_t length)
{
    lua_State * L = (lua_State *)(void *)table;
    lua_pushlstring(L, key, length);
    lua_gettable(L, LUA_GLOBALSINDEX);
    void ** v = lua_touserdata(L, -1);
    lua_pop(L, 1);
    return v;
}

void ** table_ensure(
        struct table * table,
        const char * key,
        size_t length)
{
    lua_State * L = (lua_State *)(void *)table;
    lua_pushlstring(L, key, length);
    lua_pushvalue(L, -1);
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        void ** v = lua_newuserdata(L, sizeof(*v));
        *v = NULL;
        lua_settable(L, LUA_GLOBALSINDEX);
        return v;
    } else {
        void ** v = lua_touserdata(L, -1);
        lua_pop(L, 2);
        return v;
    }
}

void * table_pop(
        struct table * table,
        const char * key,
        size_t length)
{
    lua_State * L = (lua_State *)table;
    lua_pushlstring(L, key, length);
    lua_pushvalue(L, -1);
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
        return NULL;
    }
    void ** v = lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (v) {
        lua_pushnil(L);
        lua_settable(L, LUA_GLOBALSINDEX);
        return *v;
    } else {
        return NULL;
    }
}

void table_free(
        struct table * table,
        table_iterator_t iterator)
{
    lua_State * L = (lua_State *)table;
    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX) != 0) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            void ** v = lua_touserdata(L, -1);
            if (v) {
                size_t length;
                const char * k = lua_tolstring(L, -2, &length);
                if (iterator) {
                    iterator(k, length, v);
                }
                lua_pushvalue(L, -2);
                lua_pushnil(L);
                lua_settable(L, LUA_GLOBALSINDEX);
            }
            lua_pop(L, 1);
        }
    }
    lua_close(L);
}

int table_for(
        struct table * table,
        table_iterator_t iterator)
{
    lua_State * L = (lua_State *)table;
    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX) != 0) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            void ** v = lua_touserdata(L, -1);
            if (v) {
                size_t length;
                const char * k = lua_tolstring(L, -2, &length);
                int res = iterator(k, length, v);
                if (res) {
                    lua_pop(L, 2);
                    return res;
                }
            }
            lua_pop(L, 1);
        }
    }
    return 0;
}

