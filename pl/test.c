
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
    token_NONE, 
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
void set_var_value(const char *name, VariableType type, double value, int isTrue);


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
				token.value[1] = input[pos++];
				token.type = token_RELOP;
			}
			return token;
		}

		if (input[pos] == '!') {
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

void set_var_value(const char *name, VariableType type, double value, int isTrue) {
	int index = find_or_create_variable(name);
	variables[index].type = type;
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
    cur_token = get_token();
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
        double right = factor();

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

	Variable var;
	memset(&var, 0, sizeof(Variable));

	Token next_token = lookup_next_token();
	if (next_token.type == token_AEXPROP || next_token.type == token_TERMOP) {
		double value = aexpr();
		var.value = value;
		var.type = type_NUMBER;
		return var;
	}
	
	if (next_token.type == token_RELOP) {
		int isTrue = bexpr();
		var.isTrue = isTrue;
		var.type = type_BOOLEAN;
		return var;
	}

	var.type = type_ERROR;
	return var;
}

void statement() {
    if (cur_token.type == token_VAR) {
        char varName[32];
        strcpy(varName, cur_token.value);
        match(token_VAR);
        match(token_ASSIGN);

		Variable var = expr();
		set_var_value(varName, var.type, var.value, var.isTrue);

        match(token_SEMI);
		return ;

    if (cur_token.type == token_PRINT) {
        match(token_PRINT);
		 int variable_index;
        if (cur_token.type == token_VAR) {
            variable_index =  find_variable(cur_token.value);
			if (variable_index == -1) 
				error("Variable not found");
            match(token_VAR);

        }
        match(token_SEMI);

		Variable *var = &variables[variable_index];
        if (var->type == type_BOOLEAN) {
            printf("%s\n", var->isTrue ? "TRUE" : "FALSE");
        } else if (var->type == type_NUMBER) {
			printf("%.2f\n", var->value);
		}

		return ;

    } else if (cur_token.type != token_TERMINATE) {
		error("syntax error!!");
    }
	}
}

void program() {
	if(input[pos] == '\0')
		return;

    cur_token = get_token();
    while (cur_token.type != token_TERMINATE) {
        if (cur_token.type == token_ERROR) {
            printf("syntax error!!\n");
            return;
        }
        statement();
    }
	return ;
}

// 공백 문자를 제거합니다.(탭 포함) 개행이 있다면 널문자로 바꿔줍니다.
char* remove_space(char* str) {
	int i = 0, j = 0;
	while (str[i] != '\0') {
		if (str[i] == ' ' || str[i] == '\t') {
			i++;
			continue;
		}
		if (str[i] == '\n') {
			str[j] = '\0';
			break;
		}
		str[j] = str[i];
		i++;
		j++;
	}
	str[j] = '\0';
	return str;
}

int is_valid_input(char* str) {
    // 문자열이 비어있거나 공백만 있는 경우
    if (str[0] == '\0' || str[0] == '\n') return 0;

    // 문자열에 ANSI 이스케이프 시퀀스가 있는지 검사
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\033') { // '\033'은 ESC, ANSI 이스케이프 시퀀스의 시작
            return 0;
        }

        // 제어 문자 검사 (0x00-0x1f 범위, 0x7f (DEL))
        if ((unsigned char)str[i] <= 0x1f || (unsigned char)str[i] == 0x7f) {
            return 0;
        }
    }

    return 1; // 입력이 유효
}


int main() {

	char temp[INPUT_LEN];

	while(1) {
    	printf(">> ");
		// if (!fgets(temp, INPUT_LEN, stdin)) {
        // 	printf("EOF detected or error reading input. Exiting.\n");
        //     break; 
        // }

		// input = remove_space(temp);
		// if (!is_valid_input(temp)) {
		// 	printf("Invalid input. Please try again.\n");
		// 	continue;
		// }
		input = malloc(INPUT_LEN);
		strcpy(input, "../");
    	pos = 0;
    	program();
	}

    return 0;
}

