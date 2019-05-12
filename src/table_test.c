#include <stdio.h>
#include <ini/table.h>

#include <time.h>
#include <stdlib.h>

//extern size_t table_counter;

#include <lua5.1/lua.h>

static int get_usage(struct table * table)
{
    lua_State * L = (lua_State *)table;
    return lua_gc(L, LUA_GCCOUNT, 0) * 1024;
}


int main(int argc, char ** argv)
{
    srand(time(NULL));
    struct table * table = table_alloc();

    int total = 0;
    char s[256];
    for (size_t i = 0; i < 1000000; i++) {
        int r[4];
        r[0] = rand();
        r[1] = rand();
        r[2] = rand();
        r[3] = rand();
        int n = snprintf(s, 256, "%.*s\n", 16, (char *)&r);
        total += n + 8;
        table_ensure(table, s, n);
    }

    int table_counter = get_usage(table);

    printf("%d, %d\n", table_counter, total);
    printf("%d\n", table_counter / total);

    table_free(table, NULL);

    return 0;
}
