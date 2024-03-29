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
#include "ini/ini.h"
#include "ini/map.h"
#include "ini/opts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
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
*/

/*
static int inc = 0;

static int opt_print(const char * key, const char * value, void * data)
{
    printf("%s %s = %s\n", (char*)data, key, value);
    return 0;
}

static int opt_print2(const char * key, const char * value, void * data)
{
    printf("%d %s = %s\n", inc++, key, value);
    return 0;
}

int main(int argc, char ** argv)
{
    return ini_handle(
            stdin,
            "test: ",
            INI_STRICT,
            "", INI_REQUIRED | INI_STRICT,
                "foo", INI_OPTIONAL, opt_print, "test",
                "bar", INI_SKIP, opt_print, "test2",
                NULL,
            "test", INI_SKIP | INI_STRICT,
                "pants", INI_REQUIRED, opt_print2, NULL,
                "geeg", INI_SKIP, opt_print2, NULL,
                NULL,
            NULL
        );
}*/

/*
static size_t arrays(const char * key, size_t length, char *** indices)
{
    size_t n = 0;
    for (size_t i = 0; i < length - 2; i++) {
        if (key[i] == '[') {
            size_t j;
            for (j = i + 1; j < length; j++) {
                if (key[j] == ']') {
                    *indices = realloc(*indices, sizeof(**indices) * (n + 1));
                    (*indices)[n] = malloc(j - i + 1);
                    memcpy((*indices)[n], &key[i + 1], j - i - 1);
                    (*indices)[n][j - i - 1] = '\0';
                    n++;
                    break;
                }
            }
            i = j;
        }
    }
    return n;
}

int main(int argc, char ** argv)
{
    struct table * table = ini_table_read(NULL, stdin);

    struct table * table2 = table_alloc();
    struct table * section2 = NULL;
    (void)section2;

    int key_iter(const char * key, size_t length, void ** value)
    {
        //const char * s = *(const char **)value;
        char ** indices = NULL;
        size_t n = arrays(key, length, &indices);
        printf("n = %zu\n", n);
        for (size_t i = 0; i < n; i++) {
            printf("[%zu] = %s\n", i, indices[i]);
            free(indices[i]);
        }
        if (indices) free(indices);
        return 0;
    }

    int sec_iter(const char * name, size_t length, void ** value)
    {
        struct table * section = *(struct table **)value;
        section2 = (*(struct table **)table_ensure(table2, name, length) = table_alloc());
        table_for(section, key_iter);
        return 0;
    }

    table_for(table, sec_iter);
    ini_table_free(table);
}
*/

int main(int argc, char ** argv)
{
    struct table * table = table_alloc();

    void ** d = tablex_ensure(table, "foo", 3LU, "bar", 3LU, "snrk", 4LU, NULL);
    *d = (void *)7;
    d = tablex_get(table, "foo", 0LU, "bar", 0LU, "snrk", 0LU, NULL);
    printf("%ld\n", (long)*d);

    int sec_iter(const char * key, size_t length, void ** ignore)
    {
        printf("sec %s\n", key);
        return 0;
    }

    int iter(const char * key, size_t length, void ** valp)
    {
        printf("key %s\n", key);
        return 0;
    }

    tablex_for(table, iter, sec_iter);

}
