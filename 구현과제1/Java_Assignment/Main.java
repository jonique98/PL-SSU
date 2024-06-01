package Java_Assignment;

import java.util.Scanner;

enum TokenType {
    token_NONE, token_NUMBER, token_VAR, token_AEXPROP, token_TERMOP,
    token_ASSIGN, token_RELOP, token_PRINT, token_TERMINATE,
    token_ERROR, token_SEMI
}

enum VariableType {
    type_NONE, type_NUMBER, type_BOOLEAN, type_ERROR
}

public class Main {
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
                System.out.println("Syntax error!!\n\tInvalid input. Exiting.");
                continue;
            }

			input = temp;

            program();
        }
        scanner.close();
    }


	private static void set_error_message(String bnf, String error_value, String bnf_type) {
		if (error_value == null || error_value.isEmpty()) {
			error_value = "empty string";
		}

		variables[var_pos].error_message = String.format("\t%s Expected %s but current Token is %s", bnf, bnf_type, error_value);
	}

	private static boolean syntaxError() {
		if(variables[var_pos].type == VariableType.type_ERROR)
			return true;
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

		variables[var_pos].error_message = String.format("\t%s Expected %s but current Token is %s", bnf, bnf_type, error_value);
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
			variables[var_pos].error_message = "\t Unexpected Token";
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
			set_error_message("<bexpr> → <number> <relop> <number>", cur_token.value, "<number>");
			return false;
		}

		Token op = cur_token;
		cur_token = getToken();
		int right = number();
		if (syntaxError()) {
			set_error_message("<bexpr> → <number> <relop> <number>", cur_token.value, "<number>");
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

			if (match(TokenType.token_VAR, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var>") == false) {
				return;
			}

			if (match(TokenType.token_ASSIGN, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "=") == false) {
				return;
			}

			expr();

			match(TokenType.token_SEMI, "<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, ";");
			return;
		}

		if (cur_token.type == TokenType.token_PRINT) {
			cur_token = getToken();

			if (cur_token.type != TokenType.token_VAR) {
				error("print <var>\n", cur_token.value, "<var>");
				return;
			}

			int variable_index = find_variable(cur_token.value);

			cur_token = getToken();

			if (match(TokenType.token_SEMI, "<statement> → <var> = <expr> ; | print <var>;", cur_token.value, ";") == false) {
				return;
			}

			if (variable_index == -1) {
				System.out.println("print variable not found it's not syntax error but it's runtime error");
				return;
			}
			Variable var = variables[variable_index];

			if (var.type == VariableType.type_NUMBER) {
				result_buffer.append(String.format("%s = %d\n", var.name, var.value));
			} else if (var.type == VariableType.type_BOOLEAN) {
				result_buffer.append(String.format("%s = %s\n", var.name, var.isTrue ? "TRUE" : "FALSE"));
			}
			return;
		}

		if (cur_token.type != TokenType.token_TERMINATE) {
			error("<statement> → <var> = <expr> ; | print <var> ;\n", cur_token.value, "<var> or print");
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
				System.out.printf("Syntax error!!\n");
				if(variables[findErrorIndex()].error_message != null)
					System.out.printf("%s\n", variables[findErrorIndex()].error_message);
				return;
			}
		}
		System.out.print(result_buffer.toString());
		return;
    }

}

