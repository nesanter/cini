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
#include "map.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

const char * argp_program_version = VERSION;
const char * argp_program_bug_address =
    "Noah Santer <ncwbbzp9@protonmail.com>";

static char doc[] =
    "ini-reduce -- merge INI file(s)";
static char args_doc[] =
    "INPUT [...]";

static struct argp_option options[] = {
    { "out", 'o', "FILE", 0, "Write output to FILE" },
    { "default", 'd', "KEY", OPTION_ARG_OPTIONAL, "Explicit default section, optionally of name KEY" },
    { 0 }
};

static struct {
    FILE * out;
    char * default_section;
} args = {
    0
};

void map_out(struct ini_map_root * map, FILE * f) {
    void key_iter(const char * key, char ** value) {
        fprintf(f, "%s = %s\n", key, *value);
    }

    void sec_iter(const char * name, struct ini_map_section * sec)
    {
        if (name[0]) {
            fprintf(f, "[%s]\n", name);
        } else if (args.default_section) {
            fprintf(f, "[%s]\n", args.default_section);
        }
        ini_map_section_for(sec, key_iter);
        fprintf(f, "\n");
    }

    ini_map_for(map, sec_iter);
}

static error_t parse_opt(int key, char * arg, struct argp_state * state)
{
    struct ini_map_root * map = state->input;
    FILE * in;
    switch (key) {
        case 'o': // out
            if (args.out) {
                argp_failure(state, 1, 0, "output already specified");
            }
            args.out = fopen(arg, "w");
            if (!args.out) {
                argp_failure(state, 1, errno, "error opening %s for writing", arg);
            }
            break;
        case 'd': // default-section
            if (arg) {
                args.default_section = strdup(arg);
            } else {
                args.default_section = "";
            }
            break;
            /*
        case 1001: //blank-deletes
            args.blank_deletes = true;
            break;
            */
        case ARGP_KEY_ARG:
            in = fopen(arg, "r");
            if (!in) {
                argp_failure(state, 1, errno, "error opening %s for reading", arg);
            }
            ini_map_read(map, in);
            fclose(in);
            break;
        case ARGP_KEY_NO_ARGS:
            ini_map_read(map, stdin);
            break;
        case ARGP_KEY_SUCCESS:
            if (args.out) {
                map_out(map, args.out);
                fclose(args.out);
            } else {
                map_out(map, stdout);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int main(int argc, char ** argv)
{
    void * base = NULL;
    struct ini_map_root * map = ini_map_create(base);

    static struct argp argp = {
        .options = options,
        .parser = &parse_opt,
        .args_doc = args_doc,
        .doc = doc
    };

    argp_parse(&argp, argc, argv, 0, 0, map);

    ini_map_free(map);

    return 0;
}

