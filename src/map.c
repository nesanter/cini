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
#include <string.h>
#include <assert.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "map.h"
#include "table/table.h"

struct table * ini_table_read(FILE * file, struct table * table)
{
    if (!table) {
        table = table_alloc();
    }

    size_t n = 0, length;
    char * line = NULL;
    struct ini_event ev;

    struct ini_entry ** valp;
    struct table ** secp = (struct table**)table_ensure(table, (uint8_t *)"", 0);
    if (!*secp) {
        *secp = table_alloc();
    }

    while ((length = getline(&line, &n, file)) != -1) {
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
                break;
            case INI_EVENT_COMMENT:
                break;
            case INI_EVENT_SECTION:
                secp = (struct table**)table_ensure(
                        table, (uint8_t*)ev.args[0], ev.length[0]);
                if (!*secp) *secp = table_alloc();
                break;
            case INI_EVENT_KEY_VALUE:
                valp = (struct ini_entry **)table_ensure(
                        *secp, (uint8_t*)ev.args[0], ev.length[0]);
                *valp = realloc(*valp, sizeof(size_t) + ev.length[1] + 1);
                (*valp)->length = ev.length[1];
                memcpy((*valp)->data, ev.args[1], ev.length[1]);
                (*valp)->data[ev.length[1]] = '\0';
                break;
            case INI_EVENT_KEY_ONLY:
                valp = (struct ini_entry **)table_pop(
                        *secp, (uint8_t*)ev.args[0], ev.length[0]);
                if (valp) free(valp);
                break;
        }
    }
    free(line);
    return table;
}

void ini_section_free(struct table * table)
{
    int key_iter(const uint8_t * key, size_t length, void ** value)
    {
        if (*value) free(*value);
        return 0;
    }
    table_free(table, key_iter);
}

void ini_table_free(struct table * table)
{
    int key_iter(const uint8_t * key, size_t length, void ** value)
    {
        if (*value) free(*value);
        return 0;
    }

    int sec_iter(const uint8_t * key, size_t length, void ** value)
    {
        if (*value) {
            table_free(*(struct table **)value, key_iter);
        }
        return 0;
    }

    table_free(table, sec_iter);
}

