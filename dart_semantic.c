#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARIABLES 100
#define MAX_FUNCTIONS 50
#define MAX_SCOPES 10
#define MAX_NAME_LEN 50

typedef struct {
    char name[MAX_NAME_LEN];
    char type[MAX_NAME_LEN];
    int scopeLevel;
    int isConstant;
    int isInitialized;
} Variable;

Variable symbolTable[MAX_VARIABLES];
int varCount = 0;
int currentScope = 0;

typedef struct {
    char name[MAX_NAME_LEN];
    int paramCount;
    char returnType[MAX_NAME_LEN];
} Function;

Function functionTable[MAX_FUNCTIONS];
int funcCount = 0;

typedef struct {
    int level;
    int varIndices[MAX_VARIABLES];
    int varCount;
} Scope;

Scope scopeStack[MAX_SCOPES];
int scopeTop = -1;

int isVariableDeclared(const char *name) {
    for (int i = varCount - 1; i >= 0; i--) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].scopeLevel <= currentScope)
            return i;
    }
    return -1;
}

void addVariable(const char *name, const char *type, int isConstant) {
    if (isVariableDeclared(name) != -1) {
        printf(" Error: Duplicate variable declaration '%s'.\n", name);
        return;
    }
    strcpy(symbolTable[varCount].name, name);
    strcpy(symbolTable[varCount].type, type);
    symbolTable[varCount].scopeLevel = currentScope;
    symbolTable[varCount].isConstant = isConstant;
    symbolTable[varCount].isInitialized = 0;
    scopeStack[scopeTop].varIndices[scopeStack[scopeTop].varCount++] = varCount;
    varCount++;
}

void assignVariable(const char *name) {
    int index = isVariableDeclared(name);
    if (index == -1) {
        printf(" Error: Variable '%s' is used before declaration.\n", name);
        return;
    }
    if (symbolTable[index].isConstant) {
        printf(" Error: Cannot modify constant variable '%s'.\n", name);
        return;
    }
    symbolTable[index].isInitialized = 1;
}

void checkType(const char *varName, const char *expectedType) {
    int index = isVariableDeclared(varName);
    if (index == -1) {
        printf(" Error: Variable '%s' is used before declaration.\n", varName);
        return;
    }
    if (!symbolTable[index].isInitialized) {
        printf(" Warning: Variable '%s' may be uninitialized before use.\n", varName);
    }
    if (strcmp(symbolTable[index].type, expectedType) != 0) {
        printf(" Error: Type mismatch for '%s'. Expected '%s', found '%s'.\n",
               varName, expectedType, symbolTable[index].type);
    }
}

int isFunctionDeclared(const char *name) {
    for (int i = 0; i < funcCount; i++) {
        if (strcmp(functionTable[i].name, name) == 0)
            return i;
    }
    return -1;
}

void addFunction(const char *name, int paramCount, const char *returnType) {
    if (isFunctionDeclared(name) != -1) {
        printf(" Error: Duplicate function declaration '%s'.\n", name);
        return;
    }
    strcpy(functionTable[funcCount].name, name);
    functionTable[funcCount].paramCount = paramCount;
    strcpy(functionTable[funcCount].returnType, returnType);
    funcCount++;
}

void checkFunctionCall(const char *name, int paramCount) {
    int index = isFunctionDeclared(name);
    if (index == -1) {
        printf(" Error: Function '%s' is not declared before use.\n", name);
        return;
    }
    if (functionTable[index].paramCount != paramCount) {
        printf(" Error: Function '%s' expects %d parameters, but %d were provided.\n",
               name, functionTable[index].paramCount, paramCount);
    }
}

void enterScope() {
    currentScope++;
    scopeTop++;
    scopeStack[scopeTop].level = currentScope;
    scopeStack[scopeTop].varCount = 0;
}

void exitScope() {
    for (int i = 0; i < scopeStack[scopeTop].varCount; i++) {
        int varIndex = scopeStack[scopeTop].varIndices[i];
        printf(" Removing variable '%s' from scope.\n", symbolTable[varIndex].name);
    }
    scopeTop--;
    currentScope--;
}

void analyzeFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf(" Error: Could not open file '%s'.\n", filename);
        return;
    }

    printf(" Analyzing file: %s\n\n", filename);

    char line[256];
    enterScope();

    while (fgets(line, sizeof(line), file)) {
        char varType[MAX_NAME_LEN], varName[MAX_NAME_LEN];

        if (sscanf(line, "const %s %s;", varType, varName) == 2) {
            addVariable(varName, varType, 1);
        } else if (sscanf(line, "%s %s;", varType, varName) == 2) {
            addVariable(varName, varType, 0);
        } else if (sscanf(line, "%s =", varName) == 1) {
            assignVariable(varName);
        } else if (strstr(line, "{")) {
            enterScope();
        } else if (strstr(line, "}")) {
            exitScope();
        }
    }

    fclose(file);
    exitScope();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    analyzeFile(argv[1]);
    return 0;
}
