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
#include "map.h"
#include "opts.h"

int ini_handle(FILE * file, const char * prefix, enum ini_handle_opt root_opts, ...)
{
    va_list args;
    va_start(args, root_opts);

    int r = ini_vhandle(file, prefix, root_opts, args);

    va_end(args);

    return r;
}

int ini_vhandle(FILE * file, const char * prefix, enum ini_handle_opt root_opts, va_list args)
{
    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);
    ini_map_read(map, file);

    const char * name;
    const char * key;
    int r;
    while ((name = va_arg(args, const char *))) {
        enum ini_handle_opt opts = va_arg(args, enum ini_handle_opt);

        struct ini_map_section * section = ini_map_get_section(map, name);
        if (!section) {
            if (opts & REQUIRED) {
                fprintf(stderr, "%smissing required section [%s]\n", prefix, name);
                ini_map_free(map);
                return 1;
            }
            if (opts & SKIP) {
                for (;;) {
                    const char * key = va_arg(args, const char *);
                    if (!key) break;
                    enum ini_handle_opt key_opts = va_arg(args, enum ini_handle_opt);
                    void * action = va_arg(args, void *);
                    void * data = va_arg(args, ini_handle_fn);
                    (void)key_opts;
                    (void)action;
                    (void)data;
                }
                continue;
            }
        }

        while ((key = va_arg(args, const char *))) {
            enum ini_handle_opt key_opts = va_arg(args, enum ini_handle_opt);
            ini_handle_fn action = va_arg(args, ini_handle_fn);
            void * data = va_arg(args, void *);
            char ** value = NULL;
            if (section) {
                value = ini_map_section_get_value(section, key);
            }
            if (!value) {
                if (key_opts & REQUIRED) {
                    if (!section) {
                        continue;
                    }
                    fprintf(stderr, "%smissing required key %s in section [%s]\n", prefix, key, name);
                    ini_map_free(map);
                    return 1;
                }
                if (key_opts & SKIP) {
                    continue;
                }
                if (action) {
                    if ((r = action(key, NULL, data))) {
                        return r;
                    }
                }
            } else {
                if (action) {
                    if ((r = action(key, *value, data))) {
                        return r;
                    }
                }
                if (*value) {
                    free(*value);
                }
                ini_map_section_delete_key(section, key);
            }
        }

        if (opts & STRICT) {
            int unknown = 0;
            void key_iter(const char * key, char ** value)
            {
                fprintf(stderr, "%sunknown key %s in section [%s]\n", prefix, key, name);
                unknown++;
            }
            ini_map_section_for(section, key_iter);
            if (unknown) {
                //ini_map_free_section(section);
                ini_map_free(map);
                return 1;
            }
        }
        if (section) {
            ini_map_delete_section(map, name);
        }
    }

    if (root_opts & STRICT) {
        int unknown = 0;
        void sec_iter(const char * name, struct ini_map_section * section)
        {
            fprintf(stderr, "%sunknown section [%s]\n", prefix, name);
            unknown++;
        }
        ini_map_for(map, sec_iter);
        if (unknown) {
            ini_map_free(map);
            return 1;
        }
    }

    ini_map_free(map);
    return 0;
}

