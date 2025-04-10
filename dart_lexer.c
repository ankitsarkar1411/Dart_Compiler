#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100


typedef enum {
    TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_STRING,
    TOKEN_OPERATOR, TOKEN_PUNCTUATION, TOKEN_COMMENT, TOKEN_EOF
} TokenType;


typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

const char *keywords[] = {
    "if", "else", "for", "while", "return", "int", "double", "bool",
    "String", "void", "class", "null", "true", "false", "final",
    "const", "var", "new", "this", "super", "extends", "implements"
};
#define KEYWORDS_COUNT (sizeof(keywords) / sizeof(keywords[0]))


const char *operators[] = {
    "+", "-", "*", "/", "=", "==", "!=", ">", "<", ">=", "<=", "&&",
    "||", "!", "++", "--", "+=", "-=", "*=", "/=", "=>", "??", "??="
};
#define OPERATORS_COUNT (sizeof(operators) / sizeof(operators[0]))


int is_keyword(const char *word) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


int is_operator(const char *op) {
    for (int i = 0; i < OPERATORS_COUNT; i++) {
        if (strcmp(op, operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


int is_punctuation(char c) {
    return (c == ';' || c == ',' || c == '.' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']');
}


Token get_next_token(FILE *file) {
    Token token;
    int c;

    while ((c = fgetc(file)) != EOF && isspace(c));


    if (c == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        return token;
    }

    
    if (isalpha(c) || c == '_') {
        char buffer[MAX_TOKEN_LENGTH] = {c};
        int i = 1;
        while ((c = fgetc(file)) != EOF && (isalnum(c) || c == '_')) {
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        ungetc(c, file);

        if (is_keyword(buffer)) {
            token.type = TOKEN_KEYWORD;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }

        strcpy(token.value, buffer);
        return token;
    }


    if (isdigit(c)) {
        char buffer[MAX_TOKEN_LENGTH] = {c};
        int i = 1;
        int is_float = 0;
        int is_hex = (c == '0');

        if (is_hex && (c = fgetc(file)) == 'x') {
            buffer[i++] = 'x';
            while ((c = fgetc(file)) != EOF && isxdigit(c)) {
                buffer[i++] = c;
            }
        } else {
            while ((c = fgetc(file)) != EOF && (isdigit(c) || c == '.')) {
                if (c == '.') {
                    if (is_float) break; 
                    is_float = 1;
                }
                buffer[i++] = c;
            }
        }
        buffer[i] = '\0';
        ungetc(c, file);

        token.type = TOKEN_NUMBER;
        strcpy(token.value, buffer);
        return token;
    }

 
    if (c == '"' || c == '\'') {
        char delimiter = c;
        char buffer[MAX_TOKEN_LENGTH] = {c};
        int i = 1;
        while ((c = fgetc(file)) != EOF && c != delimiter) {
            buffer[i++] = c;
        }
        buffer[i] = delimiter;
        buffer[i + 1] = '\0';

        token.type = TOKEN_STRING;
        strcpy(token.value, buffer);
        return token;
    }

 
    char op_buffer[3] = {c, fgetc(file), '\0'};
    if (is_operator(op_buffer)) {
        token.type = TOKEN_OPERATOR;
        strcpy(token.value, op_buffer);
        return token;
    } else {
        ungetc(op_buffer[1], file);
        op_buffer[1] = '\0';

        if (is_operator(op_buffer)) {
            token.type = TOKEN_OPERATOR;
            strcpy(token.value, op_buffer);
            return token;
        }
    }

 
    if (is_punctuation(c)) {
        token.type = TOKEN_PUNCTUATION;
        token.value[0] = c;
        token.value[1] = '\0';
        return token;
    }

    token.type = TOKEN_EOF;
    strcpy(token.value, "EOF");
    return token;
}


void print_token(Token token) {
    const char *type_str;
    switch (token.type) {
        case TOKEN_KEYWORD: type_str = "KEYWORD"; break;
        case TOKEN_IDENTIFIER: type_str = "IDENTIFIER"; break;
        case TOKEN_NUMBER: type_str = "NUMBER"; break;
        case TOKEN_STRING: type_str = "STRING"; break;
        case TOKEN_OPERATOR: type_str = "OPERATOR"; break;
        case TOKEN_PUNCTUATION: type_str = "PUNCTUATION"; break;
        case TOKEN_COMMENT: type_str = "COMMENT"; break;
        case TOKEN_EOF: type_str = "EOF"; break;
        default: type_str = "UNKNOWN"; break;
    }
    printf("<%s, %s>\n", type_str, token.value);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    Token token;
    do {
        token = get_next_token(file);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    fclose(file);
    return 0;
}
