#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    const char *symbol;
    int token_id;
    int precedence;
    double (*eval_func)(double, double);
    void *handle;
} Plugin;

static double pow_eval(double a, double b) {
    return pow(a, b);
}

Plugin *init_plugin(void) {
    Plugin *plugin = malloc(sizeof(Plugin));
    if (!plugin) {
        fprintf(stderr, "Error: Memory allocation failed for plugin\n");
        return NULL;
    }
    plugin->symbol = "^";
    plugin->token_id = 0; // Set by main program
    plugin->precedence = 2; // High precedence
    plugin->eval_func = pow_eval;
    plugin->handle = NULL;
    return plugin;
}
