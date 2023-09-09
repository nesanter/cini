/* Copyright © 2023 Noah Santer <personal@mail.mossy-tech.com>
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
#include "ini/ini.h"

void ini_parse_line(
        line_t * line,
        size_t length,
        struct ini_event * ev)
{
    line_t * end = &line[length - 1];
    // strip leading space
    for (; *line == ' ' || *line == '\t' || *line == '\n'; line++);
    // strip trailing space
    for (; end > line && (*end == ' ' || *end == '\t' || *end == '\n'); end--);
    // empty?
    if (!*line || line > end) {
        *ev = (struct ini_event) { .kind = INI_EVENT_NONE };
        return;
    }
#ifndef INI_CONST_LINE
    end[1] = '\0';
#endif /* INI_CONST_LINE */
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
#ifndef INI_CONST_LINE
        end[0] = '\0';
#endif /* INI_CONST_LINE */
        *ev = (struct ini_event) {
            .kind = INI_EVENT_SECTION,
            .args = { line + 1 },
            .length = { end - line - 1 }
        };
        return;
    }
    // find equals
    line_t * val = line;
    for (; *val != '=' && val < end; val++);
    // key-value?
    if (*val == '=') {
        line_t * key_end = val - 1;
        // strip leading space (val)
        for (val++; val < end && (*val == ' ' || *val == '\t'); val++);
        // strip trailing space (key)
        for (; key_end > line && (*key_end == ' ' || *key_end == '\t'); key_end--);
#ifndef INI_CONST_LINE
        key_end[1] = '\0';
#endif /* INI_CONST_LINE */
        *ev = (struct ini_event) {
            .kind = INI_EVENT_KEY_VALUE,
            .args = { line, val },
            .length = { key_end - line + 1, end - val + 1 }
        };
        return;
    }
    // else, key only
    *ev = (struct ini_event) {
        .kind = INI_EVENT_KEY_ONLY,
        .args = { line },
        .length = { end - line + 1 }
    };
    return;
}

