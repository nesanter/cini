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
#ifndef INI_UTIL_H
#define INI_UTIL_H

#include <stdarg.h>

enum ini_handle_opt {
    OPTIONAL      = 0,
    REQUIRED      = 1,
    SKIP          = 2,
    STRICT        = 4
};

typedef int (*ini_handle_fn)(const char * key, const char * value);

int ini_handle(FILE * file, enum ini_handle_opt root_opts, ...);
int ini_vhandle(FILE * file, enum ini_handle_opt root_opts, va_list args);

#endif /* INI_UTIL_H */
