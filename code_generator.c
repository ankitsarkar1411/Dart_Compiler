#include <stdio.h>
#include <string.h>
#include <ctype.h>

int tempCount = 1;

void generate_TAC(char lhs[], char rhs[]) {
    char op1[10], op2[10], op3[10], temp1[10], temp2[10];
    int i = 0, j = 0, k = 0;

    while (rhs[i] && rhs[i] != '*') op1[j++] = rhs[i++];
    op1[j] = '\0'; i++; j = 0;
    while (rhs[i] && rhs[i] != '+') op2[j++] = rhs[i++];
    op2[j] = '\0'; i++; j = 0;
    while (rhs[i]) op3[j++] = rhs[i++];
    op3[j] = '\0';

    sprintf(temp1, "t%d", tempCount++);
    sprintf(temp2, "t%d", tempCount++);


    printf("Three Address Code:\n");
    printf("%s = %s * %s\n", temp1, op1, op2);
    printf("%s = %s + %s\n", temp2, op3, temp1);
    printf("%s = %s\n\n", lhs, temp2);

 
    printf("Assembly Code:\n");
    printf("MOV R1, %s\n", op1);
    printf("MUL R1, %s\n", op2);
    printf("MOV R2, %s\n", op3);
    printf("ADD R2, R1\n");
    printf("MOV %s, R2\n", lhs);
}

int main() {
    char input[100], lhs[10], rhs[100];
    printf("Enter Dart-style expression (e.g. a = b + c * d):\n");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    
    char noSpace[100];
    int nsIdx = 0;
    for (int i = 0; input[i]; i++)
        if (!isspace(input[i])) noSpace[nsIdx++] = input[i];
    noSpace[nsIdx] = '\0';

    char *eq = strchr(noSpace, '=');
    if (!eq) {
        printf("Invalid expression!\n");
        return 1;
    }

    strncpy(lhs, noSpace, eq - noSpace);
    lhs[eq - noSpace] = '\0';
    strcpy(rhs, eq + 1);

    generate_TAC(lhs, rhs);
    return 0;
}
