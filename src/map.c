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
#include <Judy.h>
#include <string.h>
#include <assert.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "map.h"

static size_t ini_map_max_key_length = 4096;
size_t ini_map_get_max_key_length() { return 4096; }
void ini_map_set_max_key_length(size_t n) { ini_map_max_key_length = n; }

void ini_map_free(struct ini_map_root * map) {
    void key_iter(const char * name, char ** value)
    {
        if (*value) free(*value);
    }
    void sec_iter(const char * name, struct ini_map_section * sec)
    {
        ini_map_section_for(sec, key_iter);
        JudySLFreeArray((void **)sec, NULL);
    }
    ini_map_for(map, sec_iter);
    JudySLFreeArray((void **)map, NULL);
}

struct ini_map_section * ini_map_get_section(
        struct ini_map_root * map,
        const char * section)
{
    void ** secptr = JudySLGet(*(void **)map, (const uint8_t *)section, NULL);
    assert(secptr != PJERR);
    return (struct ini_map_section *)secptr;
}

struct ini_map_section * ini_map_ensure_section(
        struct ini_map_root * map,
        const char * section)
{
    void ** secptr = JudySLIns((void **)map, (const uint8_t *)section, NULL);
    assert(secptr != PJERR);
    return (struct ini_map_section *)secptr;
}

char ** ini_map_section_get_value(
        struct ini_map_section * section,
        const char * key)
{
    void ** vptr = JudySLGet(*(void **)section, (const uint8_t *)key, NULL);
    assert(vptr != PJERR);
    return (char **)vptr;
}

char ** ini_map_section_ensure_value(
        struct ini_map_section * section,
        const char * key)
{
    void ** vptr = JudySLIns((void **)section, (const uint8_t *)key, NULL);
    assert(vptr != PJERR);
    return (char **)vptr;
}

struct ini_map_section * ini_map_first_section(
        struct ini_map_root * map,
        char * name)
{
    void ** sptr = JudySLFirst(*(void **)map, (uint8_t *)name, NULL);
    assert(sptr != PJERR);
    return (struct ini_map_section *)sptr;
}

struct ini_map_section * ini_map_next_section(
        struct ini_map_root * map,
        char * name)
{
    void ** sptr = JudySLNext(*(void **)map, (uint8_t *)name, NULL);
    assert(sptr != PJERR);
    return (struct ini_map_section *)sptr;
}

char ** ini_map_section_first_value(
        struct ini_map_section * section,
        char * key)
{
    void ** vptr = JudySLFirst(*(void **)section, (uint8_t *)key, NULL);
    assert(vptr != PJERR);
    return (char **)vptr;
}

char ** ini_map_section_next_value(
        struct ini_map_section * section,
        char * key)
{
    void ** vptr = JudySLNext(*(void **)section, (uint8_t *)key, NULL);
    assert(vptr != PJERR);
    return (char **)vptr;
}

int ini_map_section_delete_key(struct ini_map_section * section, const char * key)
{
    /*
    void ** vptr = JudySLGet(*(void **)section, (const uint8_t *)key, NULL);
    assert(vptr != PJERR);
    if (vptr) free(*vptr);
    */
    return JudySLDel((void **)section, (const uint8_t *)key, NULL);
}

int ini_map_delete_section(struct ini_map_root * map, const char * name)
{
    return JudySLDel((void **)map, (const uint8_t *)name, NULL);
}

void ini_map_free_section(struct ini_map_section * section)
{
    JudySLFreeArray((void **)section, NULL);
}

void ini_map_for(struct ini_map_root * map, void (*fn)(const char *, struct ini_map_section *))
{
    char * index = calloc(ini_map_max_key_length, 1);
    for (struct ini_map_section * s_ptr = ini_map_first_section(map, index);
         s_ptr;
         s_ptr = ini_map_next_section(map, index))
    {
        fn(index, s_ptr);
    }
    free(index);
}

void ini_map_section_for(struct ini_map_section * sec, void (*fn)(const char *, char **))
{
    char * index = calloc(ini_map_max_key_length, 1);
    for (char ** v_ptr = ini_map_section_first_value(sec, index);
         v_ptr;
         v_ptr = ini_map_section_next_value(sec, index))
    {
        fn(index, v_ptr);
    }
    free(index);
}

void ini_map_read(struct ini_map_root * map, FILE * file)
{
    struct ini_map_section * section;
    size_t n = 0, length;
    char * line = NULL, ** tmp;
    struct ini_event ev;

    section = ini_map_ensure_section(map, "");

    while ((length = getline(&line, &n, file)) != -1) {
        if (length > ini_map_max_key_length) {
            ini_map_max_key_length = length;
        }
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
                break;
            case INI_EVENT_COMMENT:
                break;
            case INI_EVENT_SECTION:
                section = ini_map_ensure_section(map, ev.args[0]);
                break;
            case INI_EVENT_KEY_VALUE:
                tmp = ini_map_section_ensure_value(section, ev.args[0]);
                if (*tmp) free(*tmp);
                *tmp = strdup(ev.args[1]);
                break;
            case INI_EVENT_REMOVE_KEY:
                tmp = ini_map_section_get_value(section, ev.args[0]);
                if (tmp) free(*tmp);
                ini_map_section_delete_key(section, ev.args[0]);
                break;
        }
    }
    free(line);
}

