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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef INCLUDE_LUAJIT
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#else
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#endif /* INCLUDE_LUAJIT */

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
    if (length == 0) length = strlen(key);

    lua_State * L = (lua_State *)(void *)table;
    lua_pushlstring(L, key, length);
    lua_gettable(L, LUA_GLOBALSINDEX);
    void ** v = lua_touserdata(L, -1);
    lua_pop(L, 1);
    return v;
}

void ** tablex_get(
        struct table * table,
        const char * key,
        size_t length,
        ...)
{
    va_list args;
    va_start(args, length);

    if (length == 0) length = strlen(key);

    lua_State * L = (void *)table;
    lua_pushlstring(L, key, length);
    lua_gettable(L, LUA_GLOBALSINDEX);

    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return NULL;
    }

    size_t n = 1;
    while ((key = va_arg(args, const char *))) {
        if (!lua_istable(L, -1)) {
            lua_pop(L, n);
            return NULL;
        }
        
        length = va_arg(args, size_t);
        if (length == 0) length = strlen(key);

        lua_pushlstring(L, key, length);
        lua_gettable(L, -2);
        n++;
    }

    void ** data = NULL;
    if (lua_isuserdata(L, -1)) {
        data = (void **)lua_touserdata(L, -1);
    }
    lua_pop(L, n);
    return data;
}

void ** table_ensure(
        struct table * table,
        const char * key,
        size_t length)
{
    if (length == 0) length = strlen(key);

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

void ** tablex_ensure(
        struct table * table,
        const char * key,
        size_t length,
        ...)
{
    va_list args;
    va_start(args, length);

    if (length == 0) length = strlen(key);

    lua_State * L = (void *)table;
    lua_pushlstring(L, key, length);
    lua_gettable(L, LUA_GLOBALSINDEX);

    size_t n = 1;
    const char * keyprev = key;
    while ((key = va_arg(args, const char *))) {
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_pushlstring(L, keyprev, length);
            lua_newtable(L);
            lua_settable(L, n == 1 ? LUA_GLOBALSINDEX : -3);
            lua_pushlstring(L, keyprev, length);
            lua_gettable(L, n == 1 ? LUA_GLOBALSINDEX : -2);
        } else if (!lua_istable(L, -1)) {
            lua_pop(L, n);
            return NULL;
        }
        keyprev = key;

        length = va_arg(args, size_t);
        if (length == 0) length = strlen(key);

        lua_pushlstring(L, key, length);
        lua_gettable(L, -2);

        n++;
    }

    void ** data = NULL;
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        n--;
        lua_pushlstring(L, keyprev, length);
        data = lua_newuserdata(L, sizeof(*data));
        *data = NULL;
        lua_settable(L, n == 0 ? LUA_GLOBALSINDEX : -3);
    } else if (lua_isuserdata(L, -1)) {
        data = (void **)lua_touserdata(L, -1);
    }
    lua_pop(L, n);
    return data;
}

void * table_pop(
        struct table * table,
        const char * key,
        size_t length)
{
    if (length == 0) length = strlen(key);

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

int tablex_for(
        struct table * table,
        table_iterator_t iterator,
        table_iterator_t sec_iterator)
{
    lua_State * L = (lua_State *)table;
    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX) != 0) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            if (lua_type(L, -1) == LUA_TTABLE) {

                int subiter()
                {
                    size_t length;
                    const char * k = lua_tolstring(L, -2, &length);
                    int res = sec_iterator(k, length, NULL);

                    if (res) {
                        lua_pop(L, 2);
                        return res;
                    }

                    lua_pushnil(L);
                    while (lua_next(L, -2)) {
                        if (lua_type(L, -1) == LUA_TTABLE) {
                            int res = subiter();
                            if (res) {
                                lua_pop(L, 2);
                                return res;
                            }
                        } else {
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
                        }
                        lua_pop(L, 1);
                    }
                    return 0;
                }

                int res = subiter();
                if (res) {
                    lua_pop(L, 2);
                    return res;
                }
            } else {
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
            }
            lua_pop(L, 1);
        }
    }

    return 0;
}

