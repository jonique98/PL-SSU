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

#define MAX_VAR 64
#define token_LEN 64
#define INPUT_LEN 256
#define BUFFER_LEN 512

typedef enum {
    token_NONE, 
	token_NUMBER, token_VAR, token_AEXPROP, token_TERMOP,
	token_ASSIGN, token_RELOP, token_PRINT, token_TERMINATE,
    token_ERROR, token_SEMI
} TokenType;

typedef enum {
	type,NONE, type_NUMBER, type_BOOLEAN, type_ERROR
} VariableType;

typedef struct _Variable{
	char name[token_LEN];
	double value;
	int isTrue;
	VariableType type;
	char error_message[100];
} Variable;

typedef struct {
    TokenType type;
    char value[token_LEN];
} Token;

char result_buffer[BUFFER_LEN];
char temp_buffer[INPUT_LEN];

Token cur_token;
char *input;
int pos = 0;
Variable variables[MAX_VAR];
int var_pos = 0;
int var_count = 1;

Token get_token();
void expr();
double aexpr();
int bexpr();
double term();
double factor();
double number();
void statement();
void program();
int match(TokenType type, const char *bnf, char* error_value, const char *bnf_type);
void error(const char *bnf, char* error_value, const char *bnf_type);
double find_var_value(const char *name);
void set_var_value(const char *name, VariableType type, double value, int isTrue);


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


void error(const char *bnf, char* error_value, const char *bnf_type) {
	if(variables[var_pos].type == type_ERROR)
		return ;
	variables[var_pos].type = type_ERROR;

	if(strlen(error_value) == 0 || error_value == NULL)
		error_value = "empty string";
	sprintf(variables[var_pos].error_message, "Syntax error!!\n\t%s Expected %s but current Token is %s", bnf, bnf_type, error_value);
}

Token lookup_next_token() {
	int temp_pos = pos;
	Token token = get_token();
	pos = temp_pos;
	return token;
}

Token get_token() {
	Token token;

		if(input[pos] == '\0') {
		token.type = token_TERMINATE;
		return token;
		}

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
				token.value[2] = '\0';
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
				token.value[0] = input[pos++];
				token.value[1] = input[pos++];
				token.value[2] = '\0';
				token.type = token_RELOP;
				return token;
			}

			token.type = token_ERROR;
			return token;
		}

		if (input[pos] == '>' || input[pos] == '<') {

			token.value[0] = input[pos++];
			token.value[1] = '\0';
			token.type = token_RELOP;

			if (input[pos] == '='){
				token.value[1] = input[pos++];
				token.value[2] = '\0';
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

		if (input[pos] == 'x' || input[pos] == 'y' || input[pos] == 'z') {
			token.value[0] = input[pos++];
			token.value[1] = '\0';
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

	token.type = token_ERROR;
	token.value[0] = input[pos];
	return token;
}

int match(TokenType type, const char *bnf, char* error_value, const char *bnf_type) {
	if(variables[var_pos].type == type_ERROR)
		return 0;

	if (cur_token.type == token_ERROR) {
		variables[var_pos].type = type_ERROR;
		sprintf(variables[var_pos].error_message, "Syntax error!!\n\t Unexpected Token");
		return 0;
	}

	if (cur_token.type == type) {
		cur_token = get_token();
		return 1;
	}
	error(bnf, error_value, bnf_type);
	return 0;
}

int syntax_error() {
	for(int i = 0; i < var_count; i++) {
		if(variables[i].type == type_ERROR) {
			return 1;
		}
	}
	return 0;
}	

int find_variable(const char *name) {
	for (int i = var_count-1; i >= 0; i--) {
		if (strcmp(variables[i].name, name) == 0){
			return i;
		}
	}
	return -1;
}

// <number> → <dec> {<dec>}
// <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
double number() {
	if(syntax_error())
		return 0;

	if(cur_token.type != token_NUMBER){
		variables[var_pos].type = type_ERROR;
		return 0;
	}

	double value = atof(cur_token.value);
    cur_token = get_token();
    return value;
}

double factor() {
	if(syntax_error())
		return 0;
	
	if(cur_token.type != token_NUMBER){
		char *error_value = cur_token.value;
		error("<factor> → <number>\n", error_value, "<number>");
		return 0;
	}

	double value = number();
	return value;
}

// 단일 항을 계산하고 결과를 반환합니다.
double term() {
    double left = factor(); // 첫 번째 팩터

	if(syntax_error())
		return 0;

    while (cur_token.type == token_TERMOP) {
        Token op = cur_token;
		cur_token = get_token();

        double right = factor();
		if(syntax_error())
			return 0;

        if (strcmp(op.value, "*") == 0) {
            left *= right;
		}

        else if (strcmp(op.value, "/") == 0) {
            left /= right;
        }
    }

    return left;
}

// <aexpr> → <term> {( + | - ) <term>}
double aexpr() {
    double result = term();
	if(syntax_error())
		return 0;

    while (cur_token.type == token_AEXPROP) {
        Token op = cur_token;
		cur_token = get_token();

        double right = term();
		if(syntax_error())
			return 0;

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
	if(syntax_error())
		return 0;

    Token op = cur_token;
	cur_token = get_token();
    double right = number();
	if(syntax_error()){
		error("<bexpr> → <number> <relop> <number>", cur_token.value, "<number>");
		return 0;
	}

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

    return 0;
}

//next token을 loopkup해서 aexpr인지 bexpr인지 판단
// <expr> → <bexpr> | <aexpr>
void expr() {

	Token next_token = lookup_next_token();
	if (next_token.type == token_AEXPROP || next_token.type == token_TERMOP || next_token.type == token_SEMI) {
		variables[var_pos].type = type_NUMBER;
		variables[var_pos].value = aexpr();
		var_pos++;
		var_count++;
		return ;
	}
	
	if (next_token.type == token_RELOP) {
		variables[var_pos].type = type_BOOLEAN;
		variables[var_pos].isTrue = bexpr();
		var_pos++;
		var_count++;
		return ;
	}

	error("<expr> → <bexpr> | <aexpr>", next_token.value, "<bexpr> or <aexpr>");
	return ;
}

//<statement> → <var> = <expr> ; | print <var> ;

void statement() {

	if(variables[var_pos].type == type_ERROR)
		return ;

    if (cur_token.type == token_VAR) {
		strcpy(variables[var_pos].name, cur_token.value);

		if(match(token_VAR, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var>") == 0)
			return;
	
		if(match(token_ASSIGN, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "=") == 0)
			return;

		expr();

		match(token_SEMI, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, ";");
		return ;
    }

	if (cur_token.type == token_PRINT) {
		cur_token = get_token();

		if(cur_token.type != token_VAR){
			error("<statement> → <var> = <expr>\n", cur_token.value, "<var>");
			return;
		}

        int variable_index =  find_variable(cur_token.value);
		cur_token = get_token();
        
		if(match(token_SEMI, "<statement> → <var> = <expr> ; | print <var>;", cur_token.value, ";") == 0)
			return;
		
		if (variable_index == -1) {
			printf ("print variable not found it's not syntax error but it's runtime error\n");
			return ;
		}

		Variable *var = &variables[variable_index];
		
		if (var->type == type_NUMBER) {
			sprintf(temp_buffer, "%s = %.1f\n", var->name, var->value);
			strcat(result_buffer, temp_buffer);
		} else if (var->type == type_BOOLEAN) {
			sprintf(temp_buffer, "%s = %s\n", var->name, var->isTrue ? "TRUE" : "FALSE");
			strcat(result_buffer, temp_buffer);
		}
		return ;
	}

	return;
}

int find_error_index() {
	for(int i = 0; i < var_count; i++) {
		if(variables[i].type == type_ERROR) {
			return i;
		}
	}
	return -1;
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
		if(syntax_error()){
			printf("%s", variables[find_error_index()].error_message);
			return ;
		}
    }
	printf("%s", result_buffer);
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
    if (str[0] == '\0' || str[0] == '\n') return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\033') {
            return 0;
        }

        if ((unsigned char)str[i] <= 0x1f || (unsigned char)str[i] == 0x7f) {
            return 0;
        }
    }

    return 1;
}

void init_static() {
	pos = 0;
	var_pos = 0;
	var_count = 1;
	memset(variables, 0, sizeof(variables));
	memset(result_buffer, 0, BUFFER_LEN);
	memset(temp_buffer, 0, INPUT_LEN);
	memset(&cur_token, 0, sizeof(Token));
}

int main() {
	char temp[INPUT_LEN];

	while(1) {
		memset(temp, 0, sizeof(temp));
    	printf(">> ");
		if (!fgets(temp, INPUT_LEN, stdin)) {
        	printf("EOF detected or error reading input. Exiting.\n");
            break; 
        }

		input = remove_space(temp);

		if(strcmp(input, "terminate") == 0)
			break;

		if (!is_valid_input(temp)) {
			printf("Invalid input. Please try again.\n");
			continue;
		}

		// input = malloc(strlen(temp) + 1);
		// strcpy(input, "printx;");
    	init_static();
    	program();
	}

    return 0;
}
