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
#include "table.h"
#include <stdlib.h>
#include <string.h>

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
    /*
    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);
    ini_map_read(map, file);
    */
    struct table * root = ini_table_read(NULL, file);

    const char * name;
    const char * key;
    int r;
    while ((name = va_arg(args, const char *))) {
        enum ini_handle_opt opts = va_arg(args, enum ini_handle_opt);

        //struct ini_map_section * section = ini_map_get_section(map, name);
        struct table * section = table_pop(root, (uint8_t*)name, strlen(name));
        if (!section) {
            if (opts & INI_REQUIRED) {
                fprintf(stderr, "%smissing required section [%s]\n", prefix, name);
                //ini_map_free(map);
                ini_table_free(root);
                return 1;
            }
            if (opts & INI_SKIP) {
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
            struct ini_entry * value = NULL;
            if (section) {
                //value = ini_map_section_get_value(section, key);
                value = table_pop(section, (uint8_t*)key, strlen(key));
            }
            if (!value) {
                if (key_opts & INI_REQUIRED) {
                    if (!section) {
                        continue;
                    }
                    fprintf(stderr, "%smissing required key %s in section [%s]\n", prefix, key, name);
                    //ini_map_free(map);
                    if (section) {
                        ini_section_free(section);
                    }
                    ini_table_free(root);
                    return 1;
                }
                if (key_opts & INI_SKIP) {
                    continue;
                }
                if (action) {
                    if ((r = action(key, NULL, data))) {
                        if (section) {
                            ini_section_free(section);
                        }
                        ini_table_free(root);
                        return r;
                    }
                }
            } else {
                if (action) {
                    if ((r = action(key, (const char*)value->data, data))) {
                        free(value);
                        ini_section_free(section);
                        ini_table_free(root);
                        return r;
                    }
                }
                free(value);
                //ini_map_section_delete_key(section, key);
            }
        }

        if (opts & INI_STRICT) {
            int unknown = 0;
            int key_iter(const uint8_t * key, size_t length, void ** value)
            {
                fprintf(stderr, "%sunknown key %.*s in section [%s]\n", prefix, (int)length, key, name);
                unknown++;
                return 0;
            }
            table_for(section, key_iter);
            if (unknown) {
                //ini_map_free_section(section);
                //ini_map_free(map);
                ini_section_free(section);
                ini_table_free(root);
                return 1;
            }
        }
        if (section) {
            ini_section_free(section);
        }
        /*
        if (section) {
            ini_map_delete_section(map, name);
        }
        */
    }

    if (root_opts & INI_STRICT) {
        int unknown = 0;
        int sec_iter(const uint8_t * name, size_t length, void ** data)
        {
            fprintf(stderr, "%sunknown section [%.*s]\n", prefix, (int)length, name);
            unknown++;
            return 0;
        }
        table_for(root, sec_iter);
        if (unknown) {
            ini_table_free(root);
            return 1;
        }
    }

    ini_table_free(root);
    return 0;
}

int ini_handle_table(struct table * table, const char * prefix, enum ini_handle_opt opts, ...)
{
    va_list args;
    va_start(args, opts);

    int r = ini_vhandle_table(table, prefix, opts, args);

    va_end(args);

    return r;
}

int ini_vhandle_table(struct table * table, const char * prefix, enum ini_handle_opt opts, va_list args)
{
    const char * key;
    int r;
    while ((key = va_arg(args, const char *))) {
        enum ini_handle_opt key_opts = va_arg(args, enum ini_handle_opt);
        ini_handle_fn action = va_arg(args, ini_handle_fn);
        void * data = va_arg(args, void *);
        struct ini_entry * value = NULL;
        value = table_pop(table, (const uint8_t *)key, strlen(key));
        if (!value) {
            if (key_opts & INI_REQUIRED) {
                fprintf(stderr, "%smissing required key %s\n", prefix, key);
                return 1;
            }
            if (key_opts & INI_SKIP) {
                continue;
            }
            if (action) {
                if ((r = action(key, NULL, data))) {
                    return r;
                }
            }
        } else {
            if (action) {
                if ((r = action(key, (const char*)value->data, data))) {
                    free(value);
                    return r;
                }
                free(value);
            }
        }
    }

    if (opts & INI_STRICT) {
        int unknown = 0;
        int key_iter(const uint8_t * key, size_t length, void ** value)
        {
            fprintf(stderr, "%sunknown key %.*s\n", prefix, (int)length, key);
            unknown++;
            return 0;
        }
        table_for(table, key_iter);
        if (unknown) {
            return 1;
        }
    }
    return 0;
}


