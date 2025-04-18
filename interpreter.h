#ifndef INTERPRETER_H
#define INTERPRETER_H

#define MAX_CODE 256
#define MAX_PLUGINS 32
#define MAX_VARIABLES 100
#define MAX_VAR_NAME 32
#define SCRIPT_DIR "scripts/"

typedef enum { NUMBER, LPAREN, RPAREN, PLUGIN, VARIABLE, EQUALS, END } TokenType;

typedef struct {
    TokenType type;
    double value; // For NUMBER
    int plugin_id; // For PLUGIN
    char name[MAX_VAR_NAME]; // For VARIABLE
} Token;

typedef struct {
    Token tokens[MAX_CODE];
    int pos;
    int token_count;
} Parser;

typedef struct {
    char name[MAX_VAR_NAME];
    double value;
} Variable;

typedef struct {
    const char *symbol; // e.g., "+"
    int token_id; // Unique token ID for plugin
    int precedence; // 0: like +,-; 1: like *,/; 2: like ^
    double (*eval_func)(double, double); // Evaluation function
    void *handle; // dlopen handle
} Plugin;

extern Plugin plugins[MAX_PLUGINS];
extern int plugin_count;
extern Variable variables[MAX_VARIABLES];
extern int var_count;

int find_variable(const char *name, double *value);
void set_variable(const char *name, double value);

#endif
