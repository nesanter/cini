#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>

#include "ini.h"

#include <jansson.h>

const char * argp_program_version = VERSION;
const char * argp_program_bug_address =
    "Noah Santer <ncwbbzp9@protonmail.com>";

static char doc[] =
    "ini-tojson -- convert INI to JSON";
static char args_doc[] =
    "INPUT [...]";

static struct argp_option options[] = {
    { "out", 'o', "FILE", 0, "Write output to FILE" },
    { "no-flatten", 1000, 0, 0, "Flatten default section" },
    { "default-section", 1001, "KEY", 0, "Use KEY for default section" },
    { "no-arrays", 1002, 0, 0, "Disable special *key interpretation" },
    { "null-deletes", 1003, 0, 0, "Use null for key deletion" },
    { 0 }
};

static struct {
    FILE * out;
    bool flatten;
    char * default_section;
    bool arrays;
    bool null_deletes;
} args = {
    .default_section = "",
    .arrays = true,
    .flatten = true
};

static json_t * root = NULL;

static void tojson(FILE * f)
{
    assert(json_typeof(root) == JSON_OBJECT);

    size_t n = 0, length;
    char * line;

    struct ini_event ev;

    json_t * default_section;
    if (args.flatten) {
        default_section = root;
    } else {
        default_section = json_object_get(root, args.default_section);
        if (!default_section) {
            default_section = json_object();
            json_object_set_new(root, args.default_section, default_section);
        }
    }

    json_t * section = default_section;
    json_t * tmp;

    while ((length = getline(&line, &n, f)) != -1) {
        ini_parse_line(line, length, &ev);
        switch (ev.kind) {
            case INI_EVENT_NONE:
            case INI_EVENT_COMMENT:
                break;
            case INI_EVENT_SECTION:
                if (ev.length[0] == 0) {
                    section = default_section;
                } else {
                    section = json_object_get(root, ev.args[0]);
                    if (!section) {
                        section = json_object();
                        json_object_set_new(root, ev.args[0], section);
                    }
                }
                break;
            case INI_EVENT_KEY_VALUE:
                if (args.arrays && ev.length[0] > 1 && ev.args[0][0] == '*') {
                    tmp = json_object_get(section, ev.args[0] + 1);
                    if (tmp && json_typeof(tmp) == JSON_ARRAY) {
                        json_array_append_new(tmp, json_string(ev.args[1]));
                    } else {
                        tmp = json_array();
                        json_array_append_new(tmp, json_string(ev.args[1]));
                        json_object_set_new(section, ev.args[0] + 1, tmp);
                    }
                } else {
                    json_object_set_new(section, ev.args[0], json_string(ev.args[1]));
                }
                break;
            case INI_EVENT_REMOVE_KEY:
                if (args.null_deletes) {
                    json_object_set_new(section, ev.args[0], json_null());
                } else {
                    json_object_del(section, ev.args[0]);
                }
                break;
        }
    }
    free(line);

    /*
    char * s = json_dumps(jroot, JSON_INDENT(2));
    printf("%s\n", s);
    free(s);

    json_decref(jroot);
    */
}

static error_t parse_opt(int key, char * arg, struct argp_state * state)
{
    FILE * in;
    char * s;
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
        case 1000: // no-flatten
            args.flatten = false;
            break;
        case 1001: // default-section
            args.default_section = strdup(arg);
            break;
        case 1002: //no-arrays
            args.arrays = false;
            break;
        case 1003: //null-deletes
            args.null_deletes = true;
            break;
        case ARGP_KEY_ARG:
            in = fopen(arg, "r");
            if (!in) {
                argp_failure(state, 1, errno, "error opening %s for reading", arg);
            }
            tojson(in);
            fclose(in);
            break;
        case ARGP_KEY_NO_ARGS:
            tojson(stdin);
            break;
        case ARGP_KEY_SUCCESS:
            if (args.out) {
                s = json_dumps(root, 0);
                fprintf(args.out, "%s\n", s);
                free(s);
                fclose(args.out);
            } else {
                s = json_dumps(root, JSON_INDENT(2));
                printf("%s\n", s);
                free(s);
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

    root = json_object();

    argp_parse(&argp, argc, argv, 0, 0, NULL);

    json_decref(root);

    return 0;
}

