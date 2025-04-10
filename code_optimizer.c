#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int is_used(char var[], FILE *in) {
    char line[256];
    rewind(in);
    while (fgets(line, sizeof(line), in)) {
        if (strstr(line, var) && !strstr(line, "int") && !strstr(line, "=")) {
            return 1;
        }
    }
    return 0;
}

void remove_whitespace(char *str) {
    char *i = str;
    char *j = str;
    while (*j != 0) {
        *i = *j++;
        if (!isspace(*i)) i++;
    }
    *i = 0;
}

int is_constant_expr(char *expr, int *result) {
    int a, b;
    char op;
    if (sscanf(expr, "%d%c%d", &a, &op, &b) == 3) {
        switch (op) {
            case '+': *result = a + b; return 1;
            case '-': *result = a - b; return 1;
            case '*': *result = a * b; return 1;
            case '/': if (b != 0) { *result = a / b; return 1; }
        }
    }
    return 0;
}

void optimize(FILE *in, FILE *out) {
    char line[256];
    rewind(in);

    while (fgets(line, sizeof(line), in)) {
        if (strstr(line, "int ")) {
            char var[50], expr[100];
            if (sscanf(line, " int %[^=]=%[^;];", var, expr) == 2) {
                remove_whitespace(expr);
                int val;
                if (is_constant_expr(expr, &val)) {
                    if (is_used(var, in)) {
                        fprintf(out, "  int %s = %d;\n", var, val);
                    }
                } else if (strstr(expr, "+0") || strstr(expr, "*1") || strstr(expr, "/1")) {
                    expr[strlen(expr) - 2] = '\0'; // remove +0 or *1 etc.
                    if (is_used(var, in)) {
                        fprintf(out, "  int %s = %s;\n", var, expr);
                    }
                } else {
                    if (is_used(var, in)) {
                        fprintf(out, "  int %s = %s;\n", var, expr);
                    }
                }
            } else {
                fputs(line, out);
            }
        } else {
            fputs(line, out);
        }
    }
}

int main() {
    char inputFile[100];
    char outputFile[] = "optimized.dart";

    printf("Enter Dart file name (e.g., sample.dart): ");
    scanf("%s", inputFile);

    FILE *in = fopen(inputFile, "r");
    if (!in) {
        printf("Could not open file %s\n", inputFile);
        return 1;
    }

    FILE *out = fopen(outputFile, "w");
    if (!out) {
        printf("Could not create output file.\n");
        return 1;
    }

    optimize(in, out);
    fclose(in);
    fclose(out);

    printf("Optimized Dart file written to: %s\n", outputFile);
    return 0;
}
