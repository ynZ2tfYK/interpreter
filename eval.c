#include "eval.h"
#include <math.h>
#include <string.h>

// Forward declaration
double eval(Parser *parser, int *is_assignment, int *error, char **error_var);

// Evaluates a factor (number, variable, or parenthesized expression)
double eval_factor(Parser *parser, int *error, char **error_var) {
    if (parser->tokens[parser->pos].type == LPAREN) {
        parser->pos++;
        double value = eval(parser, NULL, error, error_var);
        if (*error || parser->tokens[parser->pos].type != RPAREN) {
            *error = 1;
            return NAN;
        }
        parser->pos++;
        return value;
    }
    if (parser->tokens[parser->pos].type == NUMBER) {
        double value = parser->tokens[parser->pos++].value;
        return value;
    }
    if (parser->tokens[parser->pos].type == VARIABLE) {
        double value;
        char *var_name = parser->tokens[parser->pos].name;
        if (!find_variable(var_name, &value)) {
            if (!*error_var) *error_var = var_name; // Report first undefined variable
            *error = 1;
            parser->pos++;
            return NAN;
        }
        parser->pos++;
        return value;
    }
    *error = 1;
    return NAN;
}

// Evaluates high-precedence operations (plugin precedence 1 or 2)
double eval_term(Parser *parser, int *is_assignment, int *error, char **error_var) {
    double value = eval_factor(parser, error, error_var);
    if (*error) return NAN;

    while (parser->tokens[parser->pos].type == PLUGIN && plugins[parser->tokens[parser->pos].plugin_id].precedence >= 1) {
        int plugin_id = parser->tokens[parser->pos++].plugin_id;
        double next_value = eval_factor(parser, error, error_var);
        if (*error) return NAN;
        value = plugins[plugin_id].eval_func(value, next_value);
    }
    return value;
}

// Evaluates low-precedence operations (plugin precedence 0) and assignments
double eval(Parser *parser, int *is_assignment, int *error, char **error_var) {
    if (parser->tokens[parser->pos].type == VARIABLE && parser->tokens[parser->pos + 1].type == EQUALS) {
        char var_name[MAX_VAR_NAME];
        strncpy(var_name, parser->tokens[parser->pos].name, MAX_VAR_NAME);
        parser->pos += 2; // Skip VARIABLE and EQUALS
        double value = eval(parser, NULL, error, error_var);
        if (*error) return NAN;
        set_variable(var_name, value);
        if (is_assignment) *is_assignment = 1;
        return value;
    }

    double value = eval_term(parser, is_assignment, error, error_var);
    if (*error) return NAN;

    while (parser->tokens[parser->pos].type == PLUGIN && plugins[parser->tokens[parser->pos].plugin_id].precedence == 0) {
        int plugin_id = parser->tokens[parser->pos++].plugin_id;
        double next_value = eval_term(parser, is_assignment, error, error_var);
        if (*error) return NAN;
        value = plugins[plugin_id].eval_func(value, next_value);
    }
    return value;
}
