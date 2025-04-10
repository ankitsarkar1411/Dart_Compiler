#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1000
#define MAX_TOKEN_LENGTH 50

typedef enum {
    TOKEN_INT, TOKEN_STRING, TOKEN_IDENTIFIER, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, 
    TOKEN_FOR, TOKEN_VOID, TOKEN_MAIN, TOKEN_PRINT, TOKEN_LPAREN, TOKEN_RPAREN, 
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICOLON, TOKEN_ASSIGN, TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

Token tokens[MAX_TOKENS];
int tokenCount = 0;

void tokenize(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf(" Error: Could not open file '%s'.\n", filename);
        return;
    }

    char word[MAX_TOKEN_LENGTH];
    while (fscanf(file, " %s", word) != EOF) {
        Token newToken;

        if (strcmp(word, "int") == 0) newToken.type = TOKEN_INT;
        else if (strcmp(word, "String") == 0) newToken.type = TOKEN_STRING;
        else if (strcmp(word, "if") == 0) newToken.type = TOKEN_IF;
        else if (strcmp(word, "else") == 0) newToken.type = TOKEN_ELSE;
        else if (strcmp(word, "while") == 0) newToken.type = TOKEN_WHILE;
        else if (strcmp(word, "for") == 0) newToken.type = TOKEN_FOR;
        else if (strcmp(word, "void") == 0) newToken.type = TOKEN_VOID;
        else if (strcmp(word, "main") == 0) newToken.type = TOKEN_MAIN;
        else if (strcmp(word, "print") == 0) newToken.type = TOKEN_PRINT;
        else if (strcmp(word, "(") == 0) newToken.type = TOKEN_LPAREN;
        else if (strcmp(word, ")") == 0) newToken.type = TOKEN_RPAREN;
        else if (strcmp(word, "{") == 0) newToken.type = TOKEN_LBRACE;
        else if (strcmp(word, "}") == 0) newToken.type = TOKEN_RBRACE;
        else if (strcmp(word, ";") == 0) newToken.type = TOKEN_SEMICOLON;
        else if (strcmp(word, "=") == 0) newToken.type = TOKEN_ASSIGN;
        else newToken.type = TOKEN_IDENTIFIER;

        strcpy(newToken.value, word);
        tokens[tokenCount++] = newToken;
    }
    
    fclose(file);
}

void recoverMissingSemicolon(int index) {
    if (index < tokenCount - 1) {
        TokenType nextType = tokens[index + 1].type;


        if (tokens[index].type == TOKEN_IDENTIFIER || tokens[index].type == TOKEN_PRINT) {
            if (nextType != TOKEN_SEMICOLON && nextType != TOKEN_LBRACE &&
                nextType != TOKEN_IF && nextType != TOKEN_ELSE &&
                nextType != TOKEN_WHILE && nextType != TOKEN_FOR) {
                printf(" Warning: Missing semicolon after '%s'. Did you mean: '%s;'\n", 
                       tokens[index].value, tokens[index].value);
            }
        }
    }
}

void parseTokens() {
    int i, openBraces = 0, openParens = 0;

    for (i = 0; i < tokenCount; i++) {
        Token token = tokens[i];

        if (token.type == TOKEN_LBRACE) openBraces++;
        if (token.type == TOKEN_RBRACE) openBraces--;
        if (token.type == TOKEN_LPAREN) openParens++;
        if (token.type == TOKEN_RPAREN) openParens--;

        if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_PRINT) {
            recoverMissingSemicolon(i);
        }

        if (openBraces < 0) {
            printf(" Error: Unexpected closing brace '}' detected. Ignoring it.\n");
            openBraces = 0;
        }
    }

    if (openBraces > 0) {
        printf(" Error: Unmatched opening brace '{'. Suggestion: Add a closing '}'.\n");
    }
    if (openParens > 0) {
        printf(" Error: Unmatched opening parenthesis '('. Suggestion: Add ')'.\n");
    } else {
        printf(" Syntax analysis completed successfully!\n");
    }
}

int main() {
    char filename[100];

    printf("Enter the Dart file to analyze: ");
    if (scanf("%99s", filename) != 1 || strlen(filename) == 0) {
        printf("Error: No file entered. Please provide a valid Dart file.\n");
        return 1;
    }

    tokenize(filename);
    if (tokenCount > 0) {
        parseTokens();
    } else {
        printf("Error: No tokens found in the file. Please check the file content.\n");
    }

    return 0;
}
