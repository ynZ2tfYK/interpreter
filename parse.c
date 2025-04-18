#include "interpreter.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Parses input string into tokens (numbers, parentheses, plugins, variables, equals)
int parse(Parser *parser, char *code) {
    parser->pos = 0;
    parser->token_count = 0;
    char *ptr = code;

    while (*ptr && parser->token_count < MAX_CODE) {
        while (isspace(*ptr)) ptr++;

        if (isdigit(*ptr) || *ptr == '.') {
            const char *start = ptr; // Changed to const
            parser->tokens[parser->token_count].type = NUMBER;
            parser->tokens[parser->token_count].value = strtod(ptr, &ptr);
            if (ptr == start) return 0; // Invalid number
            parser->token_count++;
        }
        else if (*ptr == '(') { parser->tokens[parser->token_count++].type = LPAREN; ptr++; }
        else if (*ptr == ')') { parser->tokens[parser->token_count++].type = RPAREN; ptr++; }
        else if (*ptr == '=') { parser->tokens[parser->token_count++].type = EQUALS; ptr++; }
        else if (isalpha(*ptr)) {
            const char *start = ptr; // Changed to const
            while (isalnum(*ptr)) ptr++;
            size_t len = ptr - start;
            if (len >= MAX_VAR_NAME) return 0; // Variable name too long
            parser->tokens[parser->token_count].type = VARIABLE;
            strncpy(parser->tokens[parser->token_count].name, start, len);
            parser->tokens[parser->token_count].name[len] = '\0';
            parser->token_count++;
        }
        else {
            int matched = 0;
            for (int i = 0; i < plugin_count; i++) {
                size_t len = strlen(plugins[i].symbol);
                if (strncmp(ptr, plugins[i].symbol, len) == 0) {
                    parser->tokens[parser->token_count].type = PLUGIN;
                    parser->tokens[parser->token_count].plugin_id = i;
                    parser->token_count++;
                    ptr += len;
                    matched = 1;
                    break;
                }
            }
            if (!matched && *ptr) return 0; // Invalid character
        }
    }
    parser->tokens[parser->token_count].type = END;
    return 1;
}
