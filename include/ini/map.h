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
#include <stdint.h>

struct ini_entry {
    size_t length;
    uint8_t data[];
};

struct table * ini_table_read(FILE * file, struct table * table);
void ini_table_free(struct table * table);
void ini_section_free(struct table * table);

#endif /* INI_MAP_H */

