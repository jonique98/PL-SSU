#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_VAR 16
#define TOKEN_LEN 128
#define INPUT_LEN 512
#define BUFFER_LEN 512
#define NAME_LEN 32

typedef enum {
    TOKEN_NONE, 
    TOKEN_NUMBER, TOKEN_VAR, TOKEN_ASSIGN, TOKEN_RELOP, TOKEN_PRINT, 
    TOKEN_TERMINATE, TOKEN_ERROR, TOKEN_SEMI, TOKEN_DO, TOKEN_WHILE, 
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_INT, TOKEN_OP
} TokenType;

typedef enum {
    TYPE_NONE, TYPE_NUMBER, TYPE_BOOLEAN, TYPE_ERROR
} VariableType;

typedef struct _Variable {
    char name[NAME_LEN];
    int value;
    int isTrue;
    VariableType type;
} Variable;

typedef struct {
    TokenType type;
    char value[NAME_LEN];
} Token;

char result_buffer[BUFFER_LEN];
char temp_buffer[INPUT_LEN];

Token cur_token;
char **input_split;
int pos = 0;

Variable variables[MAX_VAR];
int var_pos = 0;
int var_count = 0;

int syntax_error_flag = 0;

Token get_token();
void program();
void statement();
void declaration();
void expr();
int a_expr();
int b_expr();
int term();
int number();
int match(TokenType type);
void error(const char *message);
int find_var_value(const char *name);
void set_var_value(const char *name, VariableType type, int value, int isTrue);

void error(const char *message) {
    syntax_error_flag = 1;
}

Token get_token() {
    Token token;
    if (input_split[pos] == NULL) {
        token.type = TOKEN_TERMINATE;
        return token;
    }

    char *current = input_split[pos];
    pos++;

    if (strcmp(current, "print") == 0) {
        strcpy(token.value, "print");
        token.type = TOKEN_PRINT;
        return token;
    }

    if (strcmp(current, "do") == 0) {
        strcpy(token.value, "do");
        token.type = TOKEN_DO;
        return token;
    }

    if (strcmp(current, "while") == 0) {
        strcpy(token.value, "while");
        token.type = TOKEN_WHILE;
        return token;
    }

    if (strcmp(current, "int") == 0) {
        strcpy(token.value, "int");
        token.type = TOKEN_INT;
        return token;
    }

    if (strcmp(current, "==") == 0) {
        strcpy(token.value, "==");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (strcmp(current, "!=") == 0) {
        strcpy(token.value, "!=");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (strcmp(current, "<=") == 0) {
        strcpy(token.value, "<=");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (strcmp(current, ">=") == 0) {
        strcpy(token.value, ">=");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (strcmp(current, "=") == 0) {
        strcpy(token.value, "=");
        token.type = TOKEN_ASSIGN;
        return token;
    }

    if (strcmp(current, "<") == 0) {
        strcpy(token.value, "<");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (strcmp(current, ">") == 0) {
        strcpy(token.value, ">");
        token.type = TOKEN_RELOP;
        return token;
    }

    if (isdigit(current[0])) {
        strcpy(token.value, current);
        token.type = TOKEN_NUMBER;
        return token;
    }

    if (isalpha(current[0])) {
        strcpy(token.value, current);
        token.type = TOKEN_VAR;
        return token;
    }

    if (strcmp(current, "+") == 0 || strcmp(current, "-") == 0 ||
        strcmp(current, "*") == 0 || strcmp(current, "/") == 0) {
        strcpy(token.value, current);
        token.type = TOKEN_OP;
        return token;
    }

    if (strcmp(current, ";") == 0) {
        strcpy(token.value, ";");
        token.type = TOKEN_SEMI;
        return token;
    }

    if (strcmp(current, "{") == 0) {
        strcpy(token.value, "{");
        token.type = TOKEN_LBRACE;
        return token;
    }

    if (strcmp(current, "}") == 0) {
        strcpy(token.value, "}");
        token.type = TOKEN_RBRACE;
        return token;
    }

    if (strcmp(current, "(") == 0) {
        strcpy(token.value, "(");
        token.type = TOKEN_LPAREN;
        return token;
    }

    if (strcmp(current, ")") == 0) {
        strcpy(token.value, ")");
        token.type = TOKEN_RPAREN;
        return token;
    }

    token.type = TOKEN_ERROR;
    strcpy(token.value, current);
    error("Unrecognized token");
    return token;
}

int match(TokenType type) {
    if (cur_token.type != type) {
        error("Syntax Error!!");
        return 0;
    } else {
        cur_token = get_token();
        return 1;
    }
}

int find_var_value(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_var_value(const char *name, VariableType type, int value, int isTrue) {
    int index = find_var_value(name);
    if (index == -1) {
        if (var_count >= MAX_VAR) {
            error("Too many variables");
            return;
        }
        index = var_count;
        var_count++;
    }
    strcpy(variables[index].name, name);
    variables[index].type = type;
    variables[index].value = value;
    variables[index].isTrue = isTrue;
}

// <number> → <dec>{<dec>}
// <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
int number() {
    int value = 0;
    for (int i = 0; i < strlen(cur_token.value); i++) {
        value = value * 10 + (cur_token.value[i] - '0');
    }
    match(TOKEN_NUMBER);
    return value;
}

int term() {
    if (cur_token.type == TOKEN_NUMBER) {
        return number();
    } else if (cur_token.type == TOKEN_VAR) {
        int index = find_var_value(cur_token.value);
        if (index == -1) {
            error("Variable not declared");
            return 0;
        }
        match(TOKEN_VAR);
        return variables[index].value;
    } else if (cur_token.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        int value = a_expr();
        match(TOKEN_RPAREN);
        return value;
    } else {
        error("Syntax Error!!");
        return 0;
    }
}

// <bexpr> → <relop> <aexpr> <aexpr>
int b_expr() {
    char relop[TOKEN_LEN];
    strcpy(relop, cur_token.value);
    match(TOKEN_RELOP);
    int value1 = a_expr();
    int value2 = a_expr();
    if (strcmp(relop, "==") == 0) {
        return value1 == value2;
    } else if (strcmp(relop, "!=") == 0) {
        return value1 != value2;
    } else if (strcmp(relop, "<") == 0) {
        return value1 < value2;
    } else if (strcmp(relop, ">") == 0) {
        return value1 > value2;
    } else if (strcmp(relop, "<=") == 0) {
        return value1 <= value2;
    } else if (strcmp(relop, ">=") == 0) {
        return value1 >= value2;
    } else {
        error("Syntax Error!!");
        return 0;
    }
}

// <aexpr> → <term> {( + | - | * | / ) <term>}
int a_expr() {
    int value = term();
    while (cur_token.type == TOKEN_OP) {
        if (strcmp(cur_token.value, "+") == 0) {
            match(TOKEN_OP);
            value += term();
        } else if (strcmp(cur_token.value, "-") == 0) {
            match(TOKEN_OP);
            value -= term();
        } else if (strcmp(cur_token.value, "*") == 0) {
            match(TOKEN_OP);
            value *= term();
        } else if (strcmp(cur_token.value, "/") == 0) {
            match(TOKEN_OP);
            value /= term();
        } else {
            error("Syntax Error!!");
            return 0;
        }
    }
    return value;
}

// <statement> → <var> = <aexpr> ; | print <bexpr> ; | print <aexpr> ; |
// do ‘ { ’ {<statement>} ‘ } ’ while ( <bexpr> ) ;
void statement() {

    if(cur_token.type == TOKEN_VAR) {
        char var_name[TOKEN_LEN];
        strcpy(var_name, cur_token.value);
        match(TOKEN_VAR);
        match(TOKEN_ASSIGN);
        int value = a_expr();
        int index = find_var_value(var_name);
        if (index == -1) {
            error("Variable not declared");
            return;
        }
        set_var_value(var_name, TYPE_NUMBER, value, 0);
        match(TOKEN_SEMI);
        return ;
    }
    
    if (cur_token.type == TOKEN_PRINT) {
        match(TOKEN_PRINT);
        if (cur_token.type == TOKEN_RELOP) {
            int value = b_expr();
            strcat(result_buffer, value == 1 ? "TRUE " : "FALSE ");
        } else {
            int value = a_expr();
            char temp[TOKEN_LEN];
            sprintf(temp, "%d ", value);
            strcat(result_buffer, temp);
        }
        match(TOKEN_SEMI);
        return ;
    }
    
    if (cur_token.type == TOKEN_DO) {
        int temp_pos = pos;
        match(TOKEN_DO);
        match(TOKEN_LBRACE);
        while (syntax_error_flag != 1 && cur_token.type != TOKEN_RBRACE) {
            statement();
        }
        match(TOKEN_RBRACE);
        match(TOKEN_WHILE);
        match(TOKEN_LPAREN);
        int value = b_expr();
        match(TOKEN_RPAREN);
        match(TOKEN_SEMI);
        if (value == 1) {
            pos = temp_pos - 1;
            cur_token = get_token();
        }
        return ;
    }

    if (cur_token.type != TOKEN_TERMINATE) {
        error("Syntax Error!!");
    }
}

void declaration() {
    match(TOKEN_INT);
    if (cur_token.type == TOKEN_VAR) {
        set_var_value(cur_token.value, TYPE_NUMBER, 0, 0);
        match(TOKEN_VAR);
        match(TOKEN_SEMI);
    } else {
        error("Syntax Error!!");
    }
}

void program() {
    cur_token = get_token();
    while (syntax_error_flag != 1 && cur_token.type == TOKEN_INT) {
        declaration();
    }

    while (syntax_error_flag != 1 && cur_token.type != TOKEN_TERMINATE) {
        statement();
    }

    if(syntax_error_flag == 1) {
        printf("Syntax Error!!\n");
    } else {
        if(strlen(result_buffer) > 0)
            printf("%s\n", result_buffer);
    }
    return ;
}

char **split(const char *str, const char *delim) {
    int count = 0;
    char **result = NULL;
    char *temp = NULL;
    char *token = NULL;
    char *str_copy = NULL;

    // Make a copy of the input string because strtok modifies the string
    str_copy = strdup(str);
    if (str_copy == NULL) {
        perror("strdup");
        return NULL;
    }

    // Count the number of tokens
    temp = strdup(str);
    if (temp == NULL) {
        perror("strdup");
        free(str_copy);
        return NULL;
    }
    token = strtok(temp, delim);
    while (token != NULL) {
        count++;
        token = strtok(NULL, delim);
    }
    free(temp);

    // Allocate memory for result
    result = malloc((count + 1) * sizeof(char *));
    if (result == NULL) {
        perror("malloc");
        free(str_copy);
        return NULL;
    }

    // Split the string and store the tokens
    count = 0;
    token = strtok(str_copy, delim);
    while (token != NULL) {
        result[count] = strdup(token);
        if (result[count] == NULL) {
            perror("strdup");
            // Free already allocated memory before returning
            for (int i = 0; i < count; i++) {
                free(result[i]);
            }
            free(result);
            free(str_copy);
            return NULL;
        }
        count++;
        token = strtok(NULL, delim);
    }
    result[count] = NULL;

    free(str_copy);
    return result;
}

void free_split_result(char **result) {
    if (result != NULL) {
        for (int i = 0; result[i] != NULL; i++) {
            free(result[i]);
        }
        free(result);
    }
}

void init_static() {
    pos = 0;
    var_pos = 0;
    var_count = 0;
    syntax_error_flag = 0;
    memset(variables, 0, sizeof(variables));
    memset(result_buffer, 0, sizeof(result_buffer));
}

int main() {
    char temp[INPUT_LEN];

    while (1) {
        memset(temp, 0, sizeof(temp));
        init_static();
        write(1, ">> ", 3);
        if (!fgets(temp, INPUT_LEN, stdin)) {
            printf("EOF detected or error reading input. Exiting.\n");
            break;
        }

        if (strcmp(temp, "terminate\n") == 0) {
            break;
        }

        input_split = split(temp, " \n");

        program();
    }
    return 0;
}
