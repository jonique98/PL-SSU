
// ▸ 입력
// - 프로그램을 실행하면 사용자는 바로 코드를 입력할 수 있도록 구현함. (엔터키
// 입력까지를 하나의 프로그램으로 인식함)
// - 입력 코드의 토큰과 토큰 사이에는 공백 문자가 들어감
// - 입력 코드의 <aexpr>의 결과값은 <number> 범위 숫자로 한정함
// ▸ 출력
// - 문법에 맞는 코드가 입력된 경우에는 다음 코드를 입력 받음
// - 문법에 맞는 코드가 입력된 경우, 출력할 결과가 있다면 출력을 수행함.
// <bexpr>의 결과는 TRUE 또는 FALSE이고, <aexpr>의 결과는 숫자임
// - 문법에 맞지 않는 코드가 입력된 경우에는 “syntax error!!”를 출력한 후, 다음
// 코드를 입력 받음
// - terminate 가 입력된 경우에는 프로그램 수행을 종료함

// recursive descent parser 계산기 구현 각 bnf에 같은 이름의 펑션이 하나씩 맵핍된다

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VAR 256
#define token_LEN 100
#define INPUT_LEN 100

typedef enum {
    token_NUMBER, token_VAR, token_AEXPROP, token_TERMOP,
	token_ASSIGN, token_RELOP, token_PRINT, token_TERMINATE,
    token_ERROR, token_SEMI
} TokenType;

typedef enum {
	type_NUMBER, type_BOOLEAN, type_ERROR
} VariableType;

typedef struct _Variable{
	char name;
	double value;
	int isTrue;
	VariableType type;
} Variable;

typedef struct {
    TokenType type;
    char value[token_LEN];
} Token;

Token cur_token;
char *input;
int pos = 0;
Variable variables[MAX_VAR];
int var_pos = 0;
int var_count = 0;

Token get_token();
Variable expr();
double aexpr();
int bexpr();
double term();
double factor();
double number();
void statement();
void program();
void match(TokenType type);
void error(const char *msg);
double find_var_value(const char *name);
void set_var_value(const char *name, double value, int isTrue);

// 스페이스 다 없애고 개행 있다면 널문자로 바꿈
char *remove_space(char *temp) {
	char *ret = (char *)malloc(strlen(temp) + 1);
	int i = 0, j = 0;
	while (temp[i] != '\0') {
		if (temp[i] != ' ' && temp[i] != '\n') {
			ret[j++] = temp[i];
		}
		i++;
	}
	ret[j] = '\0';
	return ret;
}

// get_token should split the input into meaningful tokens, based on spaces and known operators.
// <program> → {<statement>}
// <statement> → <var> = <expr> ; | print <var> ;
// <expr> → <bexpr> | <aexpr>
// <bexpr> → <number> <relop> <number>
// <relop> → == | != | < | > | <= | >=
// <aexpr> → <term> {( + | - ) <term>}
// <term> → <factor> {( * | / ) <factor>}
// <factor> → <number>
// <number> → <dec> {<dec>}
// <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
// <var> → x | y | z'

void error(const char *msg) {
	printf("%s\n", msg);
	exit(1);
}

Token lookup_next_token() {
	int temp_pos = pos;
	Token token = get_token();
	pos = temp_pos;
	return token;
}

Token get_token() {
	Token token;
	memset(token.value, 0, token_LEN);

	while (input[pos] != '\0') {

		if(input[pos] == 'p') {
			if (strlen(input) - pos < 5) {
				token.type = token_ERROR;
				return token;
			}

			if (input[pos] == 'p' && input[pos+1] == 'r' && input[pos+2] == 'i' && input[pos+3] == 'n' && input[pos+4] == 't') {
				strcpy(token.value, "print");
				token.type = token_PRINT;
				pos += 5;
				return token;
			}

			token.type = token_ERROR;
			return token;
		}

		if (input[pos] == '=') {
			token.value[0] = input[pos++];
			token.type = token_ASSIGN;
			if(input[pos] && input[pos] == '=') {
				token.value[0] = input[pos++];
				token.type = token_RELOP;
			}
			return token;
		}

		if(input[pos] == '!') {
			if (strlen(input) - pos < 2) {
				token.type = token_ERROR;
				return token;
			}

			if (input[pos] == '!' && input[pos+1] == '=') {
				int i = 0;
				while (input[pos] != ';')
					token.value[i++] = input[pos++];
				token.type = token_RELOP;
				pos += 2;
				return token;
			}

			token.type = token_ERROR;
			return token;
		}

		if (input[pos] == '>' || input[pos] == '<') {
			int i = 0;
			while (input[pos] != ';')
				token.value[i++] = input[pos++];
			token.type = token_RELOP;
			if(input[pos] && input[pos] == '=') {
				strcat(token.value, &input[pos++]);
			}
			return token;
		}

		if ('0' <= input[pos] && input[pos] <= '9') {
			int i = 0;
			while ('0' <= input[pos] && input[pos] <= '9') {
				token.value[i++] = input[pos++];
			}
			token.value[i] = '\0';
			token.type = token_NUMBER;
			return token;
		}

		if (isalpha(input[pos])) {
			if (input[pos] != 'x' && input[pos] != 'y' && input[pos] != 'z') {
				token.type = token_ERROR;
				return token;
			}

			token.value[0] = input[pos++];
			token.type = token_VAR;
			return token;
		}

		if (input[pos] == '+' || input[pos] == '-') {
			token.value[0] = input[pos++];
			token.value[1] = '\0';
			token.type = token_AEXPROP;
			return token;
		}

		if (input[pos] == '*' || input[pos] == '/') {
			token.value[0] = input[pos++];
			token.value[1] = '\0';
			token.type = token_TERMOP;
			return token;
		}

		if (input[pos] == ';') {
			token.value[0] = input[pos++];
			token.value[1] = '\0';
			token.type = token_SEMI;
			return token;
		}
	}

	if(input[pos] == '\0') {
		token.type = token_TERMINATE;
		return token;
	}

	token.type = token_ERROR;
	return token;
}

void match(TokenType type) {
	if (cur_token.type == type) {
		cur_token = get_token();
	} else {
		error("syntax error!!");
	}
}

int find_or_create_variable(const char *name) {
	for (int i = 0; i < var_count; i++) {
		if (variables[i].name == name[0]) {
			return i;
		}
	}

	if (var_count >= MAX_VAR) {
		error("Too many variables");
	}

	variables[var_count].name = name[0];
	var_count++;
	return var_count - 1;
}

int find_variable(const char *name) {
	for (int i = 0; i < var_count; i++) {
		if (strcmp(&variables[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}


void set_var_value(const char *name, double value, int isTrue) {
	int index = find_or_create_variable(name);
	variables[index].value = value;
	variables[index].isTrue = isTrue;
}


// <number> → <dec> {<dec>}
// <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
double number() {
    if (cur_token.type != token_NUMBER) {
        error("Expected a number");
    }
    double num = atof(cur_token.value);
    cur_token = get_token(); // 다음 토큰으로 이동
    return num;
}

double factor() {
	if (cur_token.type == token_NUMBER)
		return number();

	error("syntax error!!");
	return 0;
}

// 단일 항을 계산하고 결과를 반환합니다.
double term() {
    double result = factor(); // 첫 번째 팩터

    while (cur_token.type == token_TERMOP) {
        Token op = cur_token;
        match(op.type);
        double right = factor(); // 다음 팩터

        if (strcmp(op.value, "*") == 0) {
            result *= right;
        } else if (strcmp(op.value, "/") == 0) {
            if (right == 0) {
                error("Division by zero");
            }
            result /= right;
        }
    }

    return result;
}

// <aexpr> → <term> {( + | - ) <term>}
double aexpr() {
    double result = term();

    while (cur_token.type == token_AEXPROP) {
        Token op = cur_token;
        match(op.type);

        double right = term();

        if (strcmp(op.value, "+") == 0) {
            result += right;
        } else if (strcmp(op.value, "-") == 0) {
            result -= right;
        }
    }

    return result;
}

// <bexpr> → <number> <relop> <number>
int bexpr() {
    double left = number();  
    Token op = cur_token;
    match(op.type);
    double right = number();

    if (strcmp(op.value, "==") == 0) {
        return left == right;
    } else if (strcmp(op.value, "!=") == 0) {
        return left != right;
    } else if (strcmp(op.value, "<") == 0) {
        return left < right;
    } else if (strcmp(op.value, ">") == 0) {
        return left > right;
    } else if (strcmp(op.value, "<=") == 0) {
        return left <= right;
    } else if (strcmp(op.value, ">=") == 0) {
        return left >= right;
    }

    return 0; // 기본적으로 false를 반환
}

//next token을 loopkup해서 aexpr인지 bexpr인지 판단
Variable expr() {

	Token next_token = lookup_next_token();
	if (next_token.type == token_AEXPROP) {
		double value = aexpr();
		Variable var;
		var.value = value;
		var.type = type_NUMBER;
		return var;
	} else if (next_token.type == token_RELOP) {
		int isTrue = bexpr();
		Variable var;
		var.isTrue = isTrue;
		var.type = type_BOOLEAN;
		return var;
	} else {
		error("syntax error!!");
	}

	Variable var;
	var.type = type_ERROR;
	return var;
}

void statement() {
    if (cur_token.type == token_VAR) {
        char varName[32];
        strcpy(varName, cur_token.value);
        match(token_VAR);
        match(token_ASSIGN);

		if (cur_token.type != token_NUMBER)
			error("Expected a number");

		Variable var = expr();
		set_var_value(varName, var.value, var.isTrue);

        match(token_SEMI);
    } else if (cur_token.type == token_PRINT) {
        match(token_PRINT);
        if (cur_token.type == token_VAR) {
            int variable_index =  find_variable(cur_token.value);
			if (variable_index == -1) 
				error("Variable not found");

			Variable *var = &variables[variable_index];
            if (var->type == type_BOOLEAN) {
                printf("%s\n", var->isTrue ? "TRUE" : "FALSE");
            } else if (var->type == type_NUMBER) {
				printf("%f\n", var->value);
			}
            match(token_VAR);

        }
        match(token_SEMI);
    } else if (cur_token.type != token_TERMINATE) {
		error("syntax error!!");
    }
}

void program() {
    cur_token = get_token();
    while (cur_token.type != token_TERMINATE) {
        if (cur_token.type == token_ERROR) {
            printf("syntax error!!\n");
            return;
        }
        statement();
    }
}


int main() {

	char temp[INPUT_LEN];

	while(1) {
    	printf(">> ");
		fgets(temp, INPUT_LEN, stdin);

		input = remove_space(temp);
    	pos = 0;
    	program();
	}

    return 0;
}


// The rest of the functions like expr, aexpr, bexpr, term, factor, number need to be implemented.
// They will handle the computation and logical flow based on the current token.
// get_token should split the input into meaningful tokens, based on spaces and known operators.
// match should check if the current token matches the expected token and advance to the next token.
// error should handle any syntax errors by printing an error message and possibly halting execution.
