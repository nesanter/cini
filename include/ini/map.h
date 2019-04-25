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
#ifndef INI_MAP_H
#define INI_MAP_H

#include <stdio.h>
#include <stdlib.h>

struct ini_map_root;
struct ini_map_section;

size_t ini_map_get_max_key_length();
void ini_map_set_max_key_length();

#define ini_map_create(ptr) (struct ini_map_root *)&ptr

static inline struct ini_map_root * ini_map_alloc()
{
    return calloc(1, sizeof(void *));
}

void ini_map_free(
        struct ini_map_root * map);

void ini_map_read(
        struct ini_map_root * map, FILE * file);
void ini_map_write(
        struct ini_map_root * map, FILE * file);


struct ini_map_section * ini_map_get_section(
        struct ini_map_root * map, const char * section);
struct ini_map_section * ini_map_ensure_section(
        struct ini_map_root * map, const char * section);
char ** ini_map_section_get_value(
        struct ini_map_section * section, const char * key);
char ** ini_map_section_ensure_value(
        struct ini_map_section * section, const char * key);

struct ini_map_section * ini_map_first_section(
        struct ini_map_root * map, char * name);
struct ini_map_section * ini_map_next_section(
        struct ini_map_root * map, char * name);

char ** ini_map_section_first_value(
        struct ini_map_section * section, char * key);
char ** ini_map_section_next_value(
        struct ini_map_section * section, char * key);

void ini_map_for(
        struct ini_map_root * map,
        void (*fn)(const char *, struct ini_map_section *));
void ini_map_section_for(
        struct ini_map_section * sec, void (*fn)(const char *, char **));

int ini_map_section_delete_key(
        struct ini_map_section * section, const char * key);
int ini_map_delete_section(
        struct ini_map_root * map, const char * name);

void ini_map_free_section(
        struct ini_map_section * section);

#endif /* INI_MAP_H */

