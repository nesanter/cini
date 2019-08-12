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

/** the following routines implement a simple key-value table store
 *  with arbitrary (e.g. not necessarily null-terminated) blobs as
 *  keys and void pointers as values.
 *  
 *  the table may reorganize on insertion (i.e. with table_ensure)
 *  and getter functions return pointers to the values which may
 *  change if the table is reorganized; thus their lifetime should
 *  be considered until the table is modified.
 *  
 *  additionally, pointers are invalid after the table is free'd
 *
 *  the table is not reorganized when keys are popped, note that
 *  this means table memory is not reclaimed when it is shrunk.
 *
 *  there is no built-in multithreading protection in the table,
 *  thus functions that may reorganize must be externally synchronized.
 */

#include <stdint.h>
#include <stddef.h>

/** opaque table type **/
struct table;

/** iterator callback type **/
typedef int (*table_iterator_t)(const char *, size_t, void **);

/** get pointer to value in table associated with key
 *
 *  if length is zero, key is assumed to be null-terminated,
 *  otherwise length is the number of bytes
 *
 *  returns pointer to value if key is in table, NULL otherwise
 */
void ** table_get(
    struct table * table,
    const char * key,
    size_t length);

/** as table_get but for tablex format */
void ** tablex_get(
    struct table * table,
    const char * key,
    size_t length,
    ...);

/** get pointer to value in table associated with key,
 *  adding key if not already present
 *
 *  may reorganize the table invalidating previous value pointers
 *
 *  if length is zero, key is assumed to be null-terminated,
 *  otherwise length is the number of bytes
 *
 *  returns pointer to value for existing or new value,
 *  new values are initialized to NULL
 */
void ** table_ensure(
    struct table * table,
    const char * key,
    size_t length);

/** as table_ensure but for tablex format */
void ** tablex_ensure(
    struct table * table,
    const char * key,
    size_t length,
    ...);


/** remove element from table and return associated value
 *
 *  NOTE: differs from get/ensure in that pointer contents is returned;
 *        thus a non-existant key has same return as a NULL-valued key
 *
 *  if length is zero, key is assumed to be null-terminated,
 *  otherwise length is the number of bytes
 *
 *  returns value of key or NULL if key not in table
 */
void * table_pop(
    struct table * table,
    const char * key,
    size_t length);

/** TODO: tablex_pop **/

/** allocate memory for a new table **/
struct table * table_alloc();

/** free memory associated with table,
 *  running iterator (if not NULL) on each key/value pair currently in table.
 *  NOTE: the return value of the iterator is ignored unlike in table_for
 */
void table_free(
    struct table * table,
    table_iterator_t iterator);

/** TODO: tablex_free */

/** run iterator on each key/value pair currently in table,
 *  stopping on the first iterator to return a non-zero value.
 *
 *  returns: return value of last iterator run or zero if table is empty
 */
int table_for(
    struct table * table,
    table_iterator_t iterator);

/** as table_for but for tablex format */
int tablex_for(
    struct table * table,
    table_iterator_t iterator,
    table_iterator_t sec_iterator);


#endif /* TABLE_H */
