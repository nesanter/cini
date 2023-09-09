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
#include "ini/map.h"
#include "ini/table.h"

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

void table_out(struct table * table, FILE * f) {
    int key_iter(const char * key, size_t length, void ** value, void * user)
    {
        fprintf(f, "%.*s = %s\n", (int)length, key, (*(struct ini_entry **)value)->data);
        if (value) free(*value);
        return 0;
    }

    int need_newline = 0;
    int sec_iter(const char * key, size_t length, void ** value, void * user)
    {
        if (need_newline) {
            fprintf(f, "\n");
        }
        if (length > 0) {
            fprintf(f, "[%.*s]\n", (int)length, key);
        } else if (args.default_section) {
            fprintf(f, "[%s]\n", args.default_section);
        }
        //table_for(*(struct table **)value, key_iter);
        need_newline = 1;
        return 0;
    }

    tablex_for(table, key_iter, NULL, sec_iter, NULL);
    if (need_newline) {
        fprintf(f, "\n");
    }
}

static error_t parse_opt(int key, char * arg, struct argp_state * state)
{
    struct table * table = *(struct table **)state->input;
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
            ini_tablex_read(table, in);
            fclose(in);
            break;
        case ARGP_KEY_NO_ARGS:
            ini_tablex_read(table, stdin);
            break;
        case ARGP_KEY_SUCCESS:
            if (args.out) {
                table_out(table, args.out);
                fclose(args.out);
            } else {
                table_out(table, stdout);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int main(int argc, char ** argv)
{
    static struct argp argp = {
        .options = options,
        .parser = &parse_opt,
        .args_doc = args_doc,
        .doc = doc
    };

    struct table * table = table_alloc();

    argp_parse(&argp, argc, argv, 0, 0, &table);

//    ini_table_free(table);

    return 0;
}

