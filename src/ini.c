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

void ini_parse_line(
        char * line,
        size_t length,
        struct ini_event * ev)
{
    char * end = &line[length - 1];
    // strip leading space
    for (; *line == ' ' || *line == '\t'; line++);
    // strip trailing space
    for (; end > line && (*end == ' ' || *end == '\t' || *end == '\n'); end--);
    // empty?
    if (!*line || line >= end) {
        *ev = (struct ini_event) { .kind = INI_EVENT_NONE };
        return;
    }
    end[1] = '\0';
    // comment?
    if (*line == '#') {
        *ev = (struct ini_event) {
            .kind = INI_EVENT_COMMENT,
            .args = { line + 1 },
            .length = { end - line }
        };
        return;
    }
    // section?
    if (*line == '[' && *end == ']') {
        end[0] = '\0';
        *ev = (struct ini_event) {
            .kind = INI_EVENT_SECTION,
            .args = { line + 1 },
            .length = { end - line - 1 }
        };
        return;
    }
    // find equals
    char * val = line;
    for (; *val != '=' && val < end; val++);
    // key-value?
    if (*val == '=') {
        char * key_end = val - 1;
        // strip leading space (val)
        for (val++; val < end && (*val == ' ' || *val == '\t'); val++);
        // strip trailing space (key)
        for (; key_end > line && (*key_end == ' ' || *key_end == '\t'); key_end--);
        key_end[1] = '\0';
        *ev = (struct ini_event) {
            .kind = INI_EVENT_KEY_VALUE,
            .args = { line, val },
            .length = { key_end - line + 1, end - val + 1 }
        };
        return;
    }
    // else, removal
    *ev = (struct ini_event) {
        .kind = INI_EVENT_REMOVE_KEY,
        .args = { line },
        .length = { end - line }
    };
    return;
}

