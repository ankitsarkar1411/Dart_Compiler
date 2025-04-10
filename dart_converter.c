#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len == 0) return;
    if (str[len - 1] == '\n') str[len - 1] = '\0';
}

void convert_to_python(FILE *in, FILE *out) {
    char line[256];
    int in_main = 0;

    while (fgets(line, sizeof(line), in)) {
        trim_newline(line);

        if (strstr(line, "void main()")) {
            fprintf(out, "def main():\n");
            in_main = 1;
        } else if (strstr(line, "{") || strstr(line, "}")) {
            continue;
        } else if (strstr(line, "print(")) {
            fprintf(out, "    print%s\n", strchr(line, '('));
        } else if (strstr(line, "int ") || strstr(line, "double ") || strstr(line, "String ")) {
            char var[50], val[50];
            sscanf(line, "%*s %[^=]=%s;", var, val);
            fprintf(out, "    %s = %s\n", var, val);
        } else if (strchr(line, '=')) {
            char var[50], val[50];
            sscanf(line, "%[^=]=%s;", var, val);
            fprintf(out, "    %s= %s\n", var, val);
        }
    }

    if (in_main) {
        fprintf(out, "\nmain()\n");
    }
}

void convert_to_java(FILE *in, FILE *out) {
    char line[256];
    int in_main = 0;

    fprintf(out, "public class Converted {\n");
    fprintf(out, "    public static void main(String[] args) {\n");

    while (fgets(line, sizeof(line), in)) {
        trim_newline(line);

        if (strstr(line, "print(")) {
            fprintf(out, "        System.out.println%s;\n", strchr(line, '('));
        } else if (strstr(line, "int ") || strstr(line, "double ") || strstr(line, "String ")) {
            fprintf(out, "        %s;\n", line);
        } else if (strchr(line, '=') && !strstr(line, "main")) {
            fprintf(out, "        %s;\n", line);
        }
    }

    fprintf(out, "    }\n}\n");
}

void convert_to_html(FILE *in, FILE *out) {
    char line[256];

    fprintf(out, "<!DOCTYPE html>\n<html>\n<body>\n");

    while (fgets(line, sizeof(line), in)) {
        trim_newline(line);

        if (strstr(line, "print(")) {
            char *start = strchr(line, '(');
            if (start) {
                start++;
                char *end = strchr(start, ')');
                if (end) *end = '\0';
                fprintf(out, "  <p>%s</p>\n", start);
            }
        }
    }

    fprintf(out, "</body>\n</html>\n");
}

int main() {
    int choice;
    char inputFile[100];
    char pythonFile[] = "converted.py";
    char javaFile[] = "Converted.java";
    char htmlFile[] = "converted.html";

    printf("Enter Dart file name (e.g., sample.dart): ");
    scanf("%s", inputFile);

    printf("\nChoose output format:\n");
    printf("1. Convert to Python\n");
    printf("2. Convert to Java\n");
    printf("3. Convert to HTML\n");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &choice);

    FILE *in = fopen(inputFile, "r");
    if (!in) {
        printf("Error: Could not open %s\n", inputFile);
        return 1;
    }

    FILE *out = NULL;
    switch (choice) {
        case 1:
            out = fopen(pythonFile, "w");
            if (!out) { printf("Error writing Python file.\n"); return 1; }
            convert_to_python(in, out);
            printf("Converted to Python: %s\n", pythonFile);
            break;

        case 2:
            out = fopen(javaFile, "w");
            if (!out) { printf("Error writing Java file.\n"); return 1; }
            convert_to_java(in, out);
            printf("Converted to Java: %s\n", javaFile);
            break;

        case 3:
            out = fopen(htmlFile, "w");
            if (!out) { printf("Error writing HTML file.\n"); return 1; }
            convert_to_html(in, out);
            printf("Converted to HTML: %s\n", htmlFile);
            break;

        default:
            printf("Invalid choice.\n");
            fclose(in);
            return 1;
    }

    fclose(in);
    fclose(out);
    return 0;
}
