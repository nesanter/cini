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
#include "ini.h"
#include "map.h"
#include "opts.h"

#include <stdio.h>
#include <stdlib.h>

void test_print_events()
{
    size_t n = 0;
    size_t length;
    char * line;
    struct ini_event ev;
    while ((length = getline(&line, &n, stdin)) != -1) {
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
                printf("N \n");
                break;
            case INI_EVENT_COMMENT:
                printf("C %zu: #%s\n", ev.length[0], ev.args[0]);
                break;
            case INI_EVENT_SECTION:
                printf("S %zu: [%s]\n", ev.length[0], ev.args[0]);
                break;
            case INI_EVENT_KEY_VALUE:
                printf("K %zu %zu: %s = %s\n",
                        ev.length[0],
                        ev.length[1],
                        ev.args[0],
                        ev.args[1]);
                break;
            case INI_EVENT_KEY_ONLY:
                printf("D %zu: %s\n", ev.length[0], ev.args[0]);
                break;
        }
    }
    free(line);
}

void test_print_map(FILE * f)
{
    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);
    ini_map_read(map, f);

    void key_iter(const char * key, char ** value) {
        printf("%s = %s\n", key, *value);
    }

    void sec_iter(const char * name, struct ini_map_section * sec)
    {
        if (name[0]) {
            printf("[%s]\n", name);
        }
        ini_map_section_for(sec, key_iter);
        printf("\n");
    }

    ini_map_for(map, sec_iter);

    ini_map_free(map);
}

int test_query_map(const char * name, const char * key)
{
    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);
    ini_map_read(map, stdin);

    struct ini_map_section * section = ini_map_get_section(map, name);
    if (!section) {
        return 1;
    }

    char ** vptr = ini_map_section_get_value(section, key);
    if (!vptr) {
        return 1;
    }

    printf("%s\n", *vptr);

    ini_map_free(map);
    return 0;
}

int test_builtin()
{

    static char builtin[] =
        "foo = bar\n"
        "[sub]\n"
        "defaults = yes\n"
        ;

    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);

    FILE * f = fmemopen(builtin, sizeof(builtin), "r");

    ini_map_read(map, f);

    fclose(f);

    struct ini_map_section * section = ini_map_get_section(map, "");
    if (!section) {
        return 1;
    }

    char ** vptr = ini_map_section_get_value(section, "foo");
    if (!vptr) {
        return 1;
    }

    printf("foo = %s\n", *vptr);

    ini_map_free(map);
    return 0;
}


static int inc = 0;

static int opt_print(const char * key, const char * value)
{
    printf("%s = %s\n", key, value);
    return 0;
}

static int opt_print2(const char * key, const char * value)
{
    printf("%d %s = %s\n", inc++, key, value);
    return 0;
}

int main(int argc, char ** argv)
{
    return ini_handle(
            stdin,
            STRICT,
            "", REQUIRED | STRICT,
                "foo", OPTIONAL, opt_print,
                "bar", SKIP, opt_print,
                NULL,
            "test", SKIP | STRICT,
                "pants", REQUIRED, opt_print2,
                "geeg", SKIP, opt_print2,
                NULL,
            NULL
        );
}

