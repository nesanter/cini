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


struct ini_event {
    enum ini_event_kind {
        INI_EVENT_NONE,
        INI_EVENT_COMMENT,
        INI_EVENT_SECTION,
        INI_EVENT_KEY_VALUE,
        INI_EVENT_REMOVE_KEY
    } kind;
    char * args[2];
    size_t length[2];
};

void ini_parse_line(char * line, size_t length, struct ini_event * ev);


#endif /* INI_H */
