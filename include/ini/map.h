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

/* the following library routines connect INI functionality
 * to the table functionality in table.h
 *
 * they are implemented by nesting table structures such that
 * the toplevel table (representing the set of sections) has
 * values that are the tables for each section.
 *
 * the section tables have values of type struct ini_entry to facilitate
 * the INI requirement of length-bound, potentionally null-containing values.
 *
 * the zero-length toplevel key is the global table.
 *
 * the ini_XXX_free functions provided here are simplifications
 * of the general table_free function and in fact wrap that function
 * with pre-made callbacks to free the data allocated by ini_table_read
 */

#include <stdio.h>
#include <stdint.h>

/** value type of section tables */
struct ini_entry {
    size_t length;
    char data[];
};

/** read INI data from file and store it in table
 *
 *  table is allocated if NULL and the fresh table is returned,
 *  otherwise the return value is the table argument
 */
struct table * ini_table_read(
        struct table * table,
        FILE * file);

/** free both levels of table **/
void ini_table_free(struct table * table);

/** free only a section table **/
void ini_section_free(struct table * table);

#endif /* INI_MAP_H */

