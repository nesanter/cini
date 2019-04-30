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
#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>

#define TABLE_INIT (struct table *)(0);

struct table;
typedef int (*table_iterator_t)(const uint8_t *, void **);

void ** table_get(
    struct table * table,
    const uint8_t * key,
    size_t length);

void ** table_ensure(
    struct table * table,
    const uint8_t * key,
    size_t length);

void ** table_pop(
    struct table * table,
    const uint8_t * key,
    size_t length);

void table_alloc(
    struct table * table);

void table_free(
    struct table * table,
    table_iterator iterator);

int table_for(
    struct table * table,
    table_iterator_t iterator);

#endif /* TABLE_H */
