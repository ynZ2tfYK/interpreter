#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include <dirent.h>
#include "interpreter.h"
#include "eval.h"
#include "parse.h"

Plugin plugins[MAX_PLUGINS];
int plugin_count = 0;
Variable variables[MAX_VARIABLES];
int var_count = 0;

int find_variable(const char *name, double *value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            *value = variables[i].value;
            return 1;
        }
    }
    return 0;
}

void set_variable(const char *name, double value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = value;
            return;
        }
    }
    if (var_count < MAX_VARIABLES) {
        strncpy(variables[var_count].name, name, MAX_VAR_NAME - 1);
        variables[var_count].name[MAX_VAR_NAME - 1] = '\0';
        variables[var_count].value = value;
        var_count++;
    } else {
        fprintf(stderr, "Error: Maximum variables reached\n");
    }
}

static void load_plugins() {
    DIR *dir = opendir("./plugins");
    if (!dir) {
        fprintf(stderr, "Error: Could not open plugins directory\n");
        return;
    }

    struct dirent *entry;
    char path[512];
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".so") == NULL) continue;
        snprintf(path, sizeof(path), "./plugins/%s", entry->d_name);
        void *handle = dlopen(path, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Error loading plugin %s: %s\n", entry->d_name, dlerror());
            continue;
        }

        Plugin *(*init_plugin)(void) = dlsym(handle, "init_plugin");
        if (!init_plugin) {
            fprintf(stderr, "Error: No init_plugin in %s\n", entry->d_name);
            dlclose(handle);
            continue;
        }

        const Plugin *plugin = init_plugin();
        if (plugin_count < MAX_PLUGINS) {
            plugins[plugin_count] = *plugin;
            plugins[plugin_count].handle = handle;
            plugins[plugin_count].token_id = PLUGIN + plugin_count + 1;
            plugin_count++;
        } else {
            fprintf(stderr, "Error: Max plugins reached\n");
            dlclose(handle);
        }
    }
    closedir(dir);
}

static int execute(Parser *parser, const char *filename, const char *line) {
    parser->pos = 0;
    int is_assignment = 0;
    int error = 0;
    char *error_var = NULL;
    double result = eval(parser, &is_assignment, &error, &error_var);
    if (parser->tokens[parser->pos].type == END && !error) {
        if (!is_assignment && line) {
            printf("%.2f\n", result);
        }
        return 0;
    }
    if (error_var) {
        fprintf(stderr, "Error: Undefined variable %s%s%s\n",
                error_var, filename && line ? " in " : "", filename && line ? filename : "");
    } else {
        fprintf(stderr, "Error: Invalid expression%s%s\n",
                filename && line ? " in " : "", filename && line ? filename : "");
    }
    return 1;
}

static void process_script(const char *filename, Parser *parser) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open script file %s\n", filename);
        return;
    }
    char line[MAX_CODE];
    while (fgets(line, MAX_CODE, file)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '\0' || line[0] == '#') continue;
        if (!parse(parser, line)) {
            fprintf(stderr, "Error: Invalid expression in %s: %s\n", filename, line);
            continue;
        }
        execute(parser, filename, line);
    }
    fclose(file);
}

int main() {
    char code[MAX_CODE];
    Parser parser = {{{0}}, 0, 0};
    load_plugins();
    printf("Math Interpreter (type 'exit' to quit, or enter a .script file)\n");
    while (1) {
        printf("> ");
        if (!fgets(code, MAX_CODE, stdin)) break;
        code[strcspn(code, "\n")] = 0;
        if (strcmp(code, "exit") == 0) break
        size_t len = strlen(code);
        if (len >= 7 && strcmp(code + len - 7, ".script") == 0) {
            char script_path[512];
            snprintf(script_path, sizeof(script_path), "%s%s", SCRIPT_DIR, code);
            process_script(script_path, &parser);
        } else {
            if (!parse(&parser, code)) {
                printf("Error: Invalid input\n");
                continue;
            }
            if (execute(&parser, NULL, code)) {
                continue;
            }
        }
    }

    for (int i = 0; i < plugin_count; i++) {
        dlclose(plugins[i].handle);
    }
    return 0;
}
