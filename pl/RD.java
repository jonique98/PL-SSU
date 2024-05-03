// #define MAX_VAR 64
// #define token_LEN 64
// #define INPUT_LEN 256
// #define BUFFER_LEN 512

// typedef enum {
//     token_NONE, 
// 	token_NUMBER, token_VAR, token_AEXPROP, token_TERMOP,
// 	token_ASSIGN, token_RELOP, token_PRINT, token_TERMINATE,
//     token_ERROR, token_SEMI
// } TokenType;

// typedef enum {
// 	type,NONE, type_NUMBER, type_BOOLEAN, type_ERROR
// } VariableType;

// typedef struct _Variable{
// 	char name[token_LEN];
// 	int value;
// 	int isTrue;
// 	VariableType type;
// 	char error_message[100];
// } Variable;

// typedef struct {
//     TokenType type;
//     char value[token_LEN];
// } Token;

// char result_buffer[BUFFER_LEN];
// char temp_buffer[INPUT_LEN];

// Token cur_token;
// char *input;
// int pos = 0;
// Variable variables[MAX_VAR];
// int var_pos = 0;
// int var_count = 1;

// Token get_token();
// void expr();
// int aexpr();
// int bexpr();
// int term();
// int factor();
// int number();
// void statement();
// void program();
// int match(TokenType type, const char *bnf, char* error_value, const char *bnf_type);
// void error(const char *bnf, char* error_value, const char *bnf_type);
// int find_var_value(const char *name);
// void set_var_value(const char *name, VariableType type, int value, int isTrue);


// // <program> → {<statement>}
// // <statement> → <var> = <expr> ; | print <var> ;
// // <expr> → <bexpr> | <aexpr>
// // <bexpr> → <number> <relop> <number>
// // <relop> → == | != | < | > | <= | >=
// // <aexpr> → <term> {( + | - ) <term>}
// // <term> → <factor> {( * | / ) <factor>}
// // <factor> → <number>
// // <number> → <dec> {<dec>}
// // <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
// // <var> → x | y | z'


// void error(const char *bnf, char* error_value, const char *bnf_type) {
// 	if(variables[var_pos].type == type_ERROR)
// 		return ;
// 	variables[var_pos].type = type_ERROR;

// 	if(strlen(error_value) == 0 || error_value == NULL)
// 		error_value = "empty string";
// 	sprintf(variables[var_pos].error_message, "Syntax error!!\n\t%s Expected %s but current Token is %s", bnf, bnf_type, error_value);
// }

// Token lookup_next_token() {
// 	int temp_pos = pos;
// 	Token token = get_token();
// 	pos = temp_pos;
// 	return token;
// }

// Token get_token() {
// 	Token token;

// 		if(input[pos] == '\0') {
// 		token.type = token_TERMINATE;
// 		return token;
// 		}

// 		if(input[pos] == 'p') {
// 			if (strlen(input) - pos < 5) {
// 				token.type = token_ERROR;
// 				return token;
// 			}

// 			if (input[pos] == 'p' && input[pos+1] == 'r' && input[pos+2] == 'i' && input[pos+3] == 'n' && input[pos+4] == 't') {
// 				strcpy(token.value, "print");
// 				token.type = token_PRINT;
// 				pos += 5;
// 				return token;
// 			}

// 			token.type = token_ERROR;
// 			return token;
// 		}

// 		if (input[pos] == '=') {
// 			token.value[0] = input[pos++];
// 			token.type = token_ASSIGN;
// 			if(input[pos] && input[pos] == '=') {
// 				token.value[1] = input[pos++];
// 				token.value[2] = '\0';
// 				token.type = token_RELOP;
// 			}
// 			return token;
// 		}

// 		if (input[pos] == '!') {
// 			if (strlen(input) - pos < 2) {
// 				token.type = token_ERROR;
// 				return token;
// 			}

// 			if (input[pos] == '!' && input[pos+1] == '=') {
// 				token.value[0] = input[pos++];
// 				token.value[1] = input[pos++];
// 				token.value[2] = '\0';
// 				token.type = token_RELOP;
// 				return token;
// 			}

// 			token.type = token_ERROR;
// 			return token;
// 		}

// 		if (input[pos] == '>' || input[pos] == '<') {

// 			token.value[0] = input[pos++];
// 			token.value[1] = '\0';
// 			token.type = token_RELOP;

// 			if (input[pos] == '='){
// 				token.value[1] = input[pos++];
// 				token.value[2] = '\0';
// 			}

// 			return token;
// 		}

// 		if ('0' <= input[pos] && input[pos] <= '9') {
// 			int i = 0;
// 			while ('0' <= input[pos] && input[pos] <= '9') {
// 				token.value[i++] = input[pos++];
// 			}
// 			token.value[i] = '\0';
// 			token.type = token_NUMBER;
// 			return token;
// 		}

// 		if (input[pos] == 'x' || input[pos] == 'y' || input[pos] == 'z') {
// 			token.value[0] = input[pos++];
// 			token.value[1] = '\0';
// 			token.type = token_VAR;
// 			return token;
// 		}

// 		if (input[pos] == '+' || input[pos] == '-') {
// 			token.value[0] = input[pos++];
// 			token.value[1] = '\0';
// 			token.type = token_AEXPROP;
// 			return token;
// 		}

// 		if (input[pos] == '*' || input[pos] == '/') {
// 			token.value[0] = input[pos++];
// 			token.value[1] = '\0';
// 			token.type = token_TERMOP;
// 			return token;
// 		}

// 		if (input[pos] == ';') {
// 			token.value[0] = input[pos++];
// 			token.value[1] = '\0';
// 			token.type = token_SEMI;
// 			return token;
// 		}

// 	token.type = token_ERROR;
// 	token.value[0] = input[pos];
// 	return token;
// }

// int match(TokenType type, const char *bnf, char* error_value, const char *bnf_type) {
// 	if(variables[var_pos].type == type_ERROR)
// 		return 0;

// 	if (cur_token.type == token_ERROR) {
// 		variables[var_pos].type = type_ERROR;
// 		sprintf(variables[var_pos].error_message, "Syntax error!!\n\t Unexpected Token");
// 		return 0;
// 	}

// 	if (cur_token.type == type) {
// 		cur_token = get_token();
// 		return 1;
// 	}
// 	error(bnf, error_value, bnf_type);
// 	return 0;
// }

// int syntax_error() {
// 	for(int i = 0; i < var_count; i++) {
// 		if(variables[i].type == type_ERROR) {
// 			return 1;
// 		}
// 	}
// 	return 0;
// }	

// int find_variable(const char *name) {
// 	for (int i = var_count-1; i >= 0; i--) {
// 		if (strcmp(variables[i].name, name) == 0){
// 			return i;
// 		}
// 	}
// 	return -1;
// }

// void set_var_value(const char *name, VariableType type, int value, int isTrue) {
// 	variables[var_pos].type = type;
// 	variables[var_pos].value = value;
// 	variables[var_pos].isTrue = isTrue;
// 	++var_count;
// }

// // <number> → <dec> {<dec>}
// // <dec> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
// int number() {
// 	if(syntax_error())
// 		return 0;

// 	if(cur_token.type != token_NUMBER){
// 		variables[var_pos].type = type_ERROR;
// 		return 0;
// 	}

// 	int value = atof(cur_token.value);
//     cur_token = get_token();
//     return value;
// }

// int factor() {
// 	if(syntax_error())
// 		return 0;
	
// 	if(cur_token.type != token_NUMBER){
// 		char *error_value = cur_token.value;
// 		error("<factor> → <number>\n", error_value, "<number>");
// 		return 0;
// 	}

// 	int value = number();
// 	return value;
// }

// // 단일 항을 계산하고 결과를 반환합니다.
// int term() {
//     int left = factor(); // 첫 번째 팩터

// 	if(syntax_error())
// 		return 0;

//     while (cur_token.type == token_TERMOP) {
//         Token op = cur_token;
// 		cur_token = get_token();

//         int right = factor();
// 		if(syntax_error())
// 			return 0;

//         if (strcmp(op.value, "*") == 0) {
//             left *= right;
// 		}

//         else if (strcmp(op.value, "/") == 0) {
//             left /= right;
//         }
//     }

//     return left;
// }

// // <aexpr> → <term> {( + | - ) <term>}
// int aexpr() {
//     int result = term();
// 	if(syntax_error())
// 		return 0;

//     while (cur_token.type == token_AEXPROP) {
//         Token op = cur_token;
// 		cur_token = get_token();

//         int right = term();
// 		if(syntax_error())
// 			return 0;

//         if (strcmp(op.value, "+") == 0) {
//             result += right;
//         } else if (strcmp(op.value, "-") == 0) {
//             result -= right;
//         }
//     }

//     return result;
// }

// // <bexpr> → <number> <relop> <number>
// int bexpr() {
//     int left = number();
// 	if(syntax_error())
// 		return 0;

//     Token op = cur_token;
// 	cur_token = get_token();
//     int right = number();
// 	if(syntax_error()){
// 		error("<bexpr> → <number> <relop> <number>", cur_token.value, "<number>");
// 		return 0;
// 	}

//     if (strcmp(op.value, "==") == 0) {
//         return left == right;
//     } else if (strcmp(op.value, "!=") == 0) {
//         return left != right;
//     } else if (strcmp(op.value, "<") == 0) {
//         return left < right;
//     } else if (strcmp(op.value, ">") == 0) {
//         return left > right;
//     } else if (strcmp(op.value, "<=") == 0) {
//         return left <= right;
//     } else if (strcmp(op.value, ">=") == 0) {
//         return left >= right;
//     }

//     return 0;
// }

// //next token을 loopkup해서 aexpr인지 bexpr인지 판단
// // <expr> → <bexpr> | <aexpr>
// void expr() {

// 	Token next_token = lookup_next_token();
// 	if (next_token.type == token_AEXPROP || next_token.type == token_TERMOP || next_token.type == token_SEMI) {
// 		variables[var_pos].type = type_NUMBER;
// 		variables[var_pos].value = aexpr();
// 		var_pos++;
// 		var_count++;
// 		return ;
// 	}
	
// 	if (next_token.type == token_RELOP) {
// 		variables[var_pos].type = type_BOOLEAN;
// 		variables[var_pos].isTrue = bexpr();
// 		var_pos++;
// 		var_count++;
// 		return ;
// 	}

// 	error("<expr> → <bexpr> | <aexpr>", next_token.value, "<bexpr> or <aexpr>");
// 	return ;
// }

// void statement() {

// 	if(variables[var_pos].type == type_ERROR)
// 		return ;

//     if (cur_token.type == token_VAR) {
// 		strcpy(variables[var_pos].name, cur_token.value);

// 		if(match(token_VAR, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var>") == 0)
// 			return;
	
// 		if(match(token_ASSIGN, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "=") == 0)
// 			return;

// 		expr();

// 		match(token_SEMI, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, ";");
// 		return ;
//     }

// 	if (cur_token.type == token_PRINT) {
// 		cur_token = get_token();

// 		if(cur_token.type != token_VAR){
// 			error("<statement> → <var> = <expr>\n", cur_token.value, "<var>");
// 			return;
// 		}

//         int variable_index =  find_variable(cur_token.value);
// 		if (variable_index == -1) {
// 			printf ("print variable not found it's not syntax error but it's runtime error\n");
// 			return ;
// 		}

// 		Variable *var = &variables[variable_index];
// 		cur_token = get_token();
        
// 		if(match(token_SEMI, "<statement> → <var> = <expr> ; | print <var>;", cur_token.value, ";") == 0)
// 			return;
		
// 		if (var->type == type_NUMBER) {
// 			sprintf(temp_buffer, "%s = %.1f\n", var->name, var->value);
// 			strcat(result_buffer, temp_buffer);
// 		} else if (var->type == type_BOOLEAN) {
// 			sprintf(temp_buffer, "%s = %s\n", var->name, var->isTrue ? "TRUE" : "FALSE");
// 			strcat(result_buffer, temp_buffer);
// 		}
// 		return ;
// 	}

// 	if (cur_token.type != token_TERMINATE)
// 		error("<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var> or print");
// 	return;
// }

// int find_error_index() {
// 	for(int i = 0; i < var_count; i++) {
// 		if(variables[i].type == type_ERROR) {
// 			return i;
// 		}
// 	}
// 	return -1;
// }

// void program() {
// 	if(input[pos] == '\0')
// 		return;

//     cur_token = get_token();
//     while (cur_token.type != token_TERMINATE) {
//         if (cur_token.type == token_ERROR) {
//             printf("syntax error!!\n");
//             return;
//         }
//         statement();
// 		if(syntax_error()){
// 			int error_index = find_error_index();
// 			if(error_index != -1)
// 				return ;
// 			printf("%s", variables[error_index].error_message);
// 			return ;
// 		}
//     }
// 	printf("%s", result_buffer);
// 	return ;
// }

// // 공백 문자를 제거합니다.(탭 포함) 개행이 있다면 널문자로 바꿔줍니다.
// char* remove_space(char* str) {
// 	int i = 0, j = 0;
// 	while (str[i] != '\0') {
// 		if (str[i] == ' ' || str[i] == '\t') {
// 			i++;
// 			continue;
// 		}
// 		if (str[i] == '\n') {
// 			str[j] = '\0';
// 			break;
// 		}
// 		str[j] = str[i];
// 		i++;
// 		j++;
// 	}
// 	str[j] = '\0';
// 	return str;
// }

// int is_valid_input(char* str) {
//     if (str[0] == '\0' || str[0] == '\n') return 0;

//     for (int i = 0; str[i] != '\0'; i++) {
//         if (str[i] == '\033') {
//             return 0;
//         }

//         if ((unsigned char)str[i] <= 0x1f || (unsigned char)str[i] == 0x7f) {
//             return 0;
//         }
//     }

//     return 1;
// }

// void init_static() {
// 	pos = 0;
// 	var_pos = 0;
// 	var_count = 1;
// 	memset(variables, 0, sizeof(variables));
// 	memset(result_buffer, 0, BUFFER_LEN);
// 	memset(temp_buffer, 0, INPUT_LEN);
// 	memset(&cur_token, 0, sizeof(Token));
// }

// int main() {
// 	char temp[INPUT_LEN];

// 	while(1) {
// 		memset(temp, 0, sizeof(temp));
//     	printf(">> ");
// 		if (!fgets(temp, INPUT_LEN, stdin)) {
//         	printf("EOF detected or error reading input. Exiting.\n");
//             break; 
//         }

// 		input = remove_space(temp);

// 		if(strcmp(input, "terminate") == 0)
// 			break;

// 		if (!is_valid_input(temp)) {
// 			printf("Invalid input. Please try again.\n");
// 			continue;
// 		}

//     	init_static();
//     	program();
// 	}

//     return 0;
// }


import java.util.ArrayList;
import java.util.Scanner;
import java.util.List;

enum TokenType {
    token_NONE, token_NUMBER, token_VAR, token_AEXPROP, token_TERMOP,
    token_ASSIGN, token_RELOP, token_PRINT, token_TERMINATE,
    token_ERROR, token_SEMI
}

enum VariableType {
    type_NONE, type_NUMBER, type_BOOLEAN, type_ERROR
}

public class RD {
    private static final int MAX_VARIABLES = 100; // 최대 변수 수 설정

    private static Variable[] variables = new Variable[MAX_VARIABLES];

	private static int var_pos = 0; // 변수를 찾을 때 사용할 인덱스
    private static int var_count = 1; // 변수를 저장할 때 사용할 인덱스
    private static Token cur_token;

    private static String input;
    private static int pos = 0;

    private static StringBuilder result_buffer = new StringBuilder();

    static class Variable {
        String name = "";
        int value;
        boolean isTrue;
        VariableType type;
        String error_message;
    }

	static class Token {
		TokenType type;
		String value = ""; // value를 빈 문자열로 초기화
	
		public Token() {
			this.type = TokenType.token_NONE; // 초기 타입도 설정
		}
	}

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        while (true) {
			initStatic();
            System.out.print(">> ");
            String temp = scanner.nextLine(); // 입력 받기

            if (temp == null || temp.trim().isEmpty()) {
                System.out.println("No input detected or error reading input. Exiting.");
                break; // 입력 없이 엔터를 쳤을 경우 또는 입력의 끝을 감지했을 때 종료
            }

			if(temp.equals("terminate"))
				break;

            temp = removeSpace(temp); // 공백 제거
            if (!isValidInput(temp)) {
                System.out.println("Invalid input. Please try again.");
                continue;
            }

			input = temp;

            program();
        }
        scanner.close();
    }


	private static boolean syntaxError() {
		for (int i = 0; i < var_count; i++) {
			if (variables[i].type == VariableType.type_ERROR) {
				return true;
			}
		}
		return false;
	}

	private static void error (String bnf, String error_value, String bnf_type) {
		if (variables[var_pos].type == VariableType.type_ERROR) {
			return;
		}

		variables[var_pos].type = VariableType.type_ERROR;

		if (error_value == null || error_value.isEmpty()) {
			error_value = "empty string";
		}

		variables[var_pos].error_message = String.format("Syntax error!!\n\t%s Expected %s but current Token is %s", bnf, bnf_type, error_value);
	}

	private static Token getToken() {
		Token token = new Token();
	
		// 문자열의 끝을 확인합니다.
		if (pos >= input.length()) {
			token.type = TokenType.token_TERMINATE;
			return token;
		}
	
		char currentChar = input.charAt(pos);
		
		// 'print' 토큰 처리
		if (currentChar == 'p') {
			if (input.length() - pos < 5) {
				token.type = TokenType.token_ERROR;
				return token;
			}
			String nextFiveChars = input.substring(pos, pos + 5);
			if ("print".equals(nextFiveChars)) {
				token.value = "print";
				token.type = TokenType.token_PRINT;
				pos += 5;
				return token;
			}
			token.type = TokenType.token_ERROR;
			return token;
		}
	
		// '='와 '==' 토큰 처리
		if (currentChar == '=') {
			token.value = "=";
			pos++;
			token.type = TokenType.token_ASSIGN;
			if (pos < input.length() && input.charAt(pos) == '=') {
				token.value += "=";
				token.type = TokenType.token_RELOP;
				pos++;
			}
			return token;
		}
	
		// '!', '!=' 토큰 처리
		if (currentChar == '!') {
			if (input.length() - pos < 2) {
				token.type = TokenType.token_ERROR;
				return token;
			}
			if (input.charAt(pos + 1) == '=') {
				token.value = "!=";
				token.type = TokenType.token_RELOP;
				pos += 2;
				return token;
			}
			token.type = TokenType.token_ERROR;
			return token;
		}
	
		// '<', '<=', '>', '>=' 토큰 처리
		if (currentChar == '<' || currentChar == '>') {
			token.value = String.valueOf(currentChar);
			pos++;
			token.type = TokenType.token_RELOP;
			if (pos < input.length() && input.charAt(pos) == '=') {
				token.value += "=";
				pos++;
			}
			return token;
		}
	
		// 숫자 토큰 처리
		if (Character.isDigit(currentChar)) {
			StringBuilder numBuilder = new StringBuilder();
			while (pos < input.length() && Character.isDigit(input.charAt(pos))) {
				numBuilder.append(input.charAt(pos));
				pos++;
			}
			token.value = numBuilder.toString();
			token.type = TokenType.token_NUMBER;
			return token;
		}
	
		// 변수 이름 처리 ('x', 'y', 'z')
		if (currentChar == 'x' || currentChar == 'y' || currentChar == 'z') {
			token.value = String.valueOf(currentChar);
			pos++;
			token.type = TokenType.token_VAR;
			return token;
		}
	
		// 연산자 '+', '-', '*', '/' 토큰 처리
		if (currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/') {
			token.value = String.valueOf(currentChar);
			pos++;
			token.type = currentChar == '+' || currentChar == '-' ? TokenType.token_AEXPROP : TokenType.token_TERMOP;
			return token;
		}
	
		// 세미콜론 ';' 토큰 처리
		if (currentChar == ';') {
			token.value = ";";
			pos++;
			token.type = TokenType.token_SEMI;
			return token;
		}
	
		// 위의 조건에 맞지 않는 경우, 에러 토큰 처리
		token.type = TokenType.token_ERROR;
		token.value = String.valueOf(currentChar);
		pos++;
		return token;
	}

	private static boolean match (TokenType type, String bnf, String error_value, String bnf_type) {
		if (variables[var_pos].type == VariableType.type_ERROR) {
			return false;
		}

		if (cur_token.type == TokenType.token_ERROR) {
			variables[var_pos].type = VariableType.type_ERROR;
			variables[var_pos].error_message = "Syntax error!!\n\t Unexpected Token";
			return false;
		}

		if (cur_token.type == type) {
			cur_token = getToken();
			return true;
		}
		error(bnf, error_value, bnf_type);
		return false;
	}

	private static Token lookupNextToken() {
		int temp_pos = pos;
		Token token = getToken();
		pos = temp_pos;
		return token;
	}


    private static String removeSpace(String input) {
        return input.replaceAll("\\s+", ""); // 모든 공백 제거
    }

    private static boolean isValidInput(String input) {
        return !input.matches(".*[\\s\\x00-\\x1F\\x7F].*");
    }

    private static void initStatic() {

		for (int i = 0; i < MAX_VARIABLES; i++) {
			variables[i] = new Variable();
		}

		
		pos = 0;
		var_pos = 0;
		var_count = 1;
		result_buffer.setLength(0); // 결과 버퍼 초기화
		cur_token = new Token(); // 현재 토큰 초기화
		cur_token.type = TokenType.token_NONE;
		cur_token.value = "";
    }

	private static int find_variable(String name) {
		for (int i = var_count - 1; i >= 0; i--) {
			if (variables[i].name.equals(name)) {
				return i;
			}
		}
		return -1;
	}

	static boolean bexpr() {
		int left = number();
		if (syntaxError()) {
			return false;
		}

		Token op = cur_token;
		cur_token = getToken();
		int right = number();
		if (syntaxError()) {
			error("<bexpr> → <number> <relop> <number>", cur_token.value, "<number>");
			return false;
		}

		if (op.value.equals("==")) {
			return left == right;
		} else if (op.value.equals("!=")) {
			return left != right;
		} else if (op.value.equals("<")) {
			return left < right;
		} else if (op.value.equals(">")) {
			return left > right;
		} else if (op.value.equals("<=")) {
			return left <= right;
		} else if (op.value.equals(">=")) {
			return left >= right;
		}

		return false;
	}

	static int number() {
		if (syntaxError()) {
			return 0;
		}

		if (cur_token.type != TokenType.token_NUMBER) {
			variables[var_pos].type = VariableType.type_ERROR;
			return 0;
		}

		int value = Integer.parseInt(cur_token.value.trim());
		cur_token = getToken();
		return value;
	}

	static int factor() {
		if (syntaxError()) {
			return 0;
		}

		if (cur_token.type != TokenType.token_NUMBER) {
			error("<factor> → <number>\n", cur_token.value, "<number>");
			return 0;
		}

		int value = number();
		return value;
	}

	static int term() {
		int left = factor();
		if (syntaxError()) {
			return 0;
		}

		while (cur_token.type == TokenType.token_TERMOP) {
			Token op = cur_token;
			cur_token = getToken();

			int right = factor();
			if (syntaxError()) {
				return 0;
			}

			if (op.value.equals("*")) {
				left *= right;
			} else if (op.value.equals("/")) {
				left /= right;
			}
		}

		return left;
	}

	static int aexpr() {
		int result = term();
		if (syntaxError()) {
			return 0;
		}

		while (cur_token.type == TokenType.token_AEXPROP) {
			Token op = cur_token;
			cur_token = getToken();

			int right = term();
			if (syntaxError()) {
				return 0;
			}

			if (op.value.equals("+")) {
				result += right;
			} else if (op.value.equals("-")) {
				result -= right;
			}
		}

		return result;
	}

	private static void expr() {
		Token next_token = lookupNextToken();
		if (next_token.type == TokenType.token_AEXPROP || next_token.type == TokenType.token_TERMOP || next_token.type == TokenType.token_SEMI) {
			variables[var_pos].type = VariableType.type_NUMBER;
			variables[var_pos].value = aexpr();
			var_pos++;
			var_count++;
			return;
		}

		if (next_token.type == TokenType.token_RELOP) {
			variables[var_pos].type = VariableType.type_BOOLEAN;
			variables[var_pos].isTrue = bexpr();
			var_pos++;
			var_count++;
			return;
		}

		error("<expr> → <bexpr> | <aexpr>", next_token.value, "<bexpr> or <aexpr>");
		return;
	}

	private static void statement() {
		if (variables[var_pos].type == VariableType.type_ERROR) {
			return;
		}
	
		if (cur_token.type == TokenType.token_VAR) {
			variables[var_pos].name = cur_token.value;
	
			if (!match(TokenType.token_VAR, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var>"))
				return;
			
			if (!match(TokenType.token_ASSIGN, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "="))
				return;
	
			expr();
	
			if (!match(TokenType.token_SEMI, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, ";"))
				return;
		}
	
		if (cur_token.type == TokenType.token_PRINT) {
			cur_token = getToken();
	
			if (cur_token.type != TokenType.token_VAR) {
				error("<statement> → print <var> ;\n", cur_token.value, "<var>");
				return;
			}
	
			int variable_index = find_variable(cur_token.value);
			if (variable_index == -1) {
				System.out.println("Variable " + cur_token.value + " not found but it's not syntax error");
				return;
			}
	
			Variable var = variables[variable_index];
			cur_token = getToken();
	
			if (!match(TokenType.token_SEMI, "<statement> → <var> = <expr> ; | print <var>;", cur_token.value, ";")) {
				return;
			}
	
			if (var.type == VariableType.type_NUMBER) {
				result_buffer.append(var.name).append(" = ").append(var.value).append("\n");
			} else if (var.type == VariableType.type_BOOLEAN) {
				result_buffer.append(var.name).append(" = ").append(var.isTrue ? "TRUE" : "FALSE").append("\n");
			}
			return;
		}
	}

	private static int findErrorIndex() {
		for (int i = 0; i < var_count; i++) {
			if (variables[i].type == VariableType.type_ERROR) {
				return i;
			}
		}
		return -1;
	}

    private static void program() {
		if (input == null || input.isEmpty()) {
			return;
		}

		cur_token = getToken();
		while (cur_token.type != TokenType.token_TERMINATE) {
			if (cur_token.type == TokenType.token_ERROR) {
				System.out.println("Syntax error!!");
				return;
			}
			statement();
			if (syntaxError()) {
				int error_index = findErrorIndex();
				if (error_index != -1) {
					System.out.println(variables[error_index].error_message);
					return;
				}
				System.out.println(result_buffer.toString());
				return;
			}
		}
		System.out.print(result_buffer.toString());
		return;
    }

}








