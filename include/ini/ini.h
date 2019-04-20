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
#ifndef INI_H
#define INI_H

#include <stddef.h>


/* result of ini_parse_line() */
struct ini_event {
    /* type of event (args[] count)
     *  - NONE : blank (0)
     *  - COMMENT : #comment (1)
     *  - SECTION : [section] (1)
     *  - KEY_VALUE : key=value (2)
     *  - REMOVE_KEY : removal (1)
     */
    enum ini_event_kind {
        INI_EVENT_NONE,
        INI_EVENT_COMMENT,
        INI_EVENT_SECTION,
        INI_EVENT_KEY_VALUE,
        INI_EVENT_REMOVE_KEY
    } kind;
    /* args[] depends on type,
     *  - #comment : all after #
     *  - [section] : section name (no [])
     *  - key=value : key, value
     *  - removal : key
     *
     * if INI_CONST_LINE is not defined,
     * line is modified such that args[] are null terminated
     */
    line_t * args[2];
    /* length of each arg */
    size_t length[2];
};

/* parse a single line of length characters and fill *ev with event
 * line may contain embedded newlines (i.e. is not necessarily a true line)
 */
void ini_parse_line(line_t * line, size_t length, struct ini_event * ev);

#endif /* INI_H */
