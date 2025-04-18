#ifndef EVAL_H
#define EVAL_H

#include "interpreter.h"

double eval(Parser *parser, int *is_assignment, int *error, char **error_var);
double eval_term(Parser *parser, int *is_assignment, int *error, char **error_var);
double eval_factor(Parser *parser, int *error, char **error_var);

#endif
