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
#include "table/table.h"
#include "table/pearson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry {
    uint8_t * leaf_key;
    size_t leaf_length;
    void * leaf_data;
    struct table * nested;
};

struct table {
    struct pearson_data pearson_data;
    struct entry entries[256];
};

struct table * table_alloc()
{
    struct table * table = calloc(1, sizeof(*table));
    pearson_init(&table->pearson_data);
    return table;
}

void ** table_get(
    struct table * table,
    const uint8_t * key,
    size_t length)
{
    for (size_t nth = 0; ; nth++) {
        uint8_t x = pearson(&table->pearson_data, nth, key, length);
        struct entry * entry = &table->entries[x];
        if (entry->nested) {
            table = entry->nested;
        } else if (entry->leaf_key) {
            if (entry->leaf_length == length &&
                !memcmp(entry->leaf_key, key, length)) {
                return &entry->leaf_data;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
}

static void ** table_ensure_n(
    struct table * table,
    const uint8_t * key,
    size_t length,
    size_t nth_start)
{
    for (size_t nth = nth_start; ; nth++) {
        uint8_t x = pearson(&table->pearson_data, nth, key, length);
        struct entry * entry = &table->entries[x];
        if (entry->nested) {
            table = entry->nested;
        } else if (entry->leaf_key) {
            if (entry->leaf_length == length &&
                !memcmp(entry->leaf_key, key, length)) {
                return &entry->leaf_data;
            } else {
#ifndef NDEBUG
                fprintf(stderr, "growing table [%zu]\n", nth);
#endif
                entry->nested = table_alloc();
                *table_ensure_n(
                    entry->nested, entry->leaf_key, entry->leaf_length, nth) =
                    entry->leaf_data;
                //free(entry->leaf_key);
                entry->leaf_key = NULL;
                entry->leaf_length = 0;
                entry->leaf_data = NULL;
                return table_ensure_n(
                    entry->nested, key, length, nth);
            }
        } else {
            entry->leaf_key = malloc(length);
            memcpy(entry->leaf_key, key, length);
            entry->leaf_length = length;
            return &entry->leaf_data;
        }
    }
}

void ** table_ensure(
    struct table * table,
    const uint8_t * key,
    size_t length)
{
    return table_ensure_n(table, key, length, 0);
}

void * table_pop(
    struct table * table,
    const uint8_t * key,
    size_t length)
{
    for (size_t nth = 0; ; nth++) {
        uint8_t x = pearson(&table->pearson_data, nth, key, length);
        struct entry * entry = &table->entries[x];
        if (entry->nested) {
            table = entry->nested;
        } else if (entry->leaf_key) {
            if (entry->leaf_length == length &&
                !memcmp(entry->leaf_key, key, length)) {
                void * data = entry->leaf_data;
                free(entry->leaf_key);
                entry->leaf_key = NULL;
                entry->leaf_length = 0;
                entry->leaf_data = NULL;
                return data;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
}

void table_free(
    struct table * table,
    table_iterator_t iterator)
{
    for (size_t i = 0; i < 256; i++) {
        struct entry * entry = &table->entries[i];
        if (entry->nested) {
            table_free(entry->nested, iterator);
        } else if (entry->leaf_key) {
            if (iterator) {
                iterator(entry->leaf_key, entry->leaf_length, &entry->leaf_data);
            }
            free(entry->leaf_key);
        }
    }
    free(table);
}

int table_for(
    struct table * table,
    table_iterator_t iterator)
{
    for (size_t i = 0; i < 256; i++) {
        struct entry * entry = &table->entries[i];
        if (entry->nested) {
            table_for(entry->nested, iterator);
        } else if (entry->leaf_key) {
            int res = iterator(entry->leaf_key, entry->leaf_length, &entry->leaf_data);
            if (res) return res;
        }
    }
    return 0;
}
