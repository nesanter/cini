/* Copyright © 2023 Noah Santer <personal@mail.mossy-tech.com>
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
#include "ini/table.h"

#include <string.h>
#include <assert.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef NO_SYSTEM_GETLINE

#define GROW_INCREMENT (1024)
static size_t ini_getline(char ** bufout, size_t * szout, FILE * file)
{
    char * buf = *bufout;
    size_t i, sz = *szout;
    for (i = 0; ; i++) {
        if (i >= sz) {
            sz += GROW_INCREMENT;
            buf = realloc(buf, sz);
        }
        int c = fgetc(file);
        if (c == EOF) {
            if (i == 0) return -1;
            break;
        }
        buf[i] = (char)c;
        if (c == '\n') {
            break;
        }
    }
    *szout = sz;
    *bufout = buf;
    return i;
}

#else

#define ini_getline getline

#endif /* NO_SYSTEM_GETLINE */

struct table * ini_table_read(struct table * table, FILE * file)
{
    if (!table) {
        table = table_alloc();
    }

    size_t n = 0, length;
    char * line = NULL;
    struct ini_event ev;

    struct ini_entry ** valp;
    struct table ** secp = (struct table **)table_ensure(table, "", 0);
    if (!*secp) {
        *secp = table_alloc();
    }

    while ((length = ini_getline(&line, &n, file)) != -1) {
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
                break;
            case INI_EVENT_COMMENT:
                break;
            case INI_EVENT_SECTION:
                secp = (struct table**)table_ensure(
                        table, ev.args[0], ev.length[0]);
                if (!*secp) *secp = table_alloc();
                break;
            case INI_EVENT_KEY_VALUE:
                valp = (struct ini_entry **)table_ensure(
                        *secp, ev.args[0], ev.length[0]);
                *valp = realloc(*valp, sizeof(size_t) + ev.length[1] + 1);
                (*valp)->length = ev.length[1];
                memcpy((*valp)->data, ev.args[1], ev.length[1]);
                (*valp)->data[ev.length[1]] = '\0';
                break;
            case INI_EVENT_KEY_ONLY:
                valp = (struct ini_entry **)table_pop(
                        *secp, ev.args[0], ev.length[0]);
                if (valp) free(valp);
                break;
        }
    }
    free(line);
    return table;
}

struct table * ini_tablex_read(struct table * table, FILE * file)
{
    if (!table) {
        table = table_alloc();
    }

    size_t n = 0, length;
    char * line = NULL;
    struct ini_event ev;

    struct ini_entry ** valp;
    char * sec_name = strdup("");
    size_t sec_name_length = 0;

    while ((length = ini_getline(&line, &n, file)) != -1) {
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
                break;

            case INI_EVENT_COMMENT:
                break;

            case INI_EVENT_SECTION:
                if (sec_name) free(sec_name);
                sec_name = strdup(ev.args[0]);
                sec_name_length = ev.length[0];
                break;

            case INI_EVENT_KEY_VALUE:
                valp = (struct ini_entry **)tablex_ensure(
                        table,
                        sec_name, sec_name_length,
                        ev.args[0], ev.length[0],
                        NULL);
                *valp = realloc(*valp, sizeof(size_t) + ev.length[1] + 1);
                (*valp)->length = ev.length[1];
                memcpy((*valp)->data, ev.args[1], ev.length[1]);
                (*valp)->data[ev.length[1]] = '\0';
                break;

            case INI_EVENT_KEY_ONLY:
                break;
        }
    }
    free(sec_name);
    free(line);
    return table;
}

static int freeing_key_iter(const char * key, size_t length, void ** value, void * user)
{
    if (*value) free(*value);
    return 0;
}

void ini_section_free(struct table * table)
{
    table_free(table, freeing_key_iter, NULL);
}

static int freeing_sec_iter(const char * key, size_t length, void ** value, void * user)
{
    if (*value) {
        table_free(*(struct table **)value, freeing_key_iter, NULL);
    }
    return 0;
}

void ini_table_free(struct table * table)
{
    table_free(table, freeing_sec_iter, NULL);
}

