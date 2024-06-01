from enum import Enum, auto
from dataclasses import dataclass

class TokenType(Enum):
	NONE = auto()
	NUMBER = auto()
	VAR = auto()
	AEXPROP = auto()
	TERMOP = auto()
	ASSIGN = auto()
	RELOP = auto()
	PRINT = auto()
	TERMINATE = auto()
	ERROR = auto()
	SEMI = auto()

class VariableType(Enum):
	NONE = auto()
	NUMBER = auto()
	BOOLEAN = auto()
	ERROR = auto()

@dataclass
class Variable:
	name: str = ""
	value: float = 0.0
	is_true: bool = False
	type: VariableType = VariableType.NONE
	error_message: str = ""

@dataclass
class Token:
	type: TokenType = TokenType.NONE
	value: str = ""
	

class Interpreter:
	def __init__(self):
		self.pos = 0
		self.var_pos = 0
		self.var_count = 1
		self.variables = [Variable() for _ in range(64)]
		self.result_buffer = ""
		self.temp_buffer = ""
		self.cur_token = Token()
		self.input = ""

	def error(self, bnf, error_value, bnf_type):
		if self.variables[self.var_pos].type == VariableType.ERROR:
			return
		self.variables[self.var_pos].type = VariableType.ERROR
		self.variables[self.var_pos].error_message = f"Syntax error!!\n\t{bnf} Expected {bnf_type} but current Token is {error_value or 'empty string'}"

	def get_token(self):
		if self.pos >= len(self.input):
			return Token(TokenType.TERMINATE, "")

		if self.input[self.pos] == 'p':
			if len(self.input) - self.pos < 5:
				return Token(TokenType.ERROR, "")
			if self.input[self.pos:self.pos+5] == "print":
				self.pos += 5
				return Token(TokenType.PRINT, "print")
			return Token(TokenType.ERROR, "")

		if self.input[self.pos] == '=':
			self.pos += 1
			if self.pos < len(self.input) and self.input[self.pos] == '=':
				self.pos += 1
				return Token(TokenType.RELOP, "==")
			return Token(TokenType.ASSIGN, "=")

		if self.input[self.pos] == '!':
			if len(self.input) - self.pos < 2:
				return Token(TokenType.ERROR, "")
			if self.input[self.pos:self.pos+2] == "!=":
				self.pos += 2
				return Token(TokenType.RELOP, "!=")
			return Token(TokenType.ERROR, "")

		if self.input[self.pos] in {'>', '<'}:
			op = self.input[self.pos]
			self.pos += 1
			if self.pos < len(self.input) and self.input[self.pos] == '=':
				op += self.input[self.pos]
				self.pos += 1
			return Token(TokenType.RELOP, op)

		if '0' <= self.input[self.pos] <= '9':
			value = ""
			while self.pos < len(self.input) and '0' <= self.input[self.pos] <= '9':
				value += self.input[self.pos]
				self.pos += 1
			return Token(TokenType.NUMBER, value)

		if self.input[self.pos] in {'x', 'y', 'z'}:
			var = self.input[self.pos]
			self.pos += 1
			return Token(TokenType.VAR, var)

		if self.input[self.pos] in {'+', '-'}:
			op = self.input[self.pos]
			self.pos += 1
			return Token(TokenType.AEXPROP, op)

		if self.input[self.pos] in {'*', '/'}:
			op = self.input[self.pos]
			self.pos += 1
			return Token(TokenType.TERMOP, op)

		if self.input[self.pos] == ';':
			self.pos += 1
			return Token(TokenType.SEMI)
		return Token(TokenType.ERROR, "")
	
	
	def lookup_next_token(self):
		temp_pos = self.pos
		token = self.get_token()
		self.pos = temp_pos
		return token
	
	def match(self, expected_type, bnf, error_value, bnf_type):
		if self.variables[self.var_pos].type == VariableType.ERROR:
			return False

		if self.cur_token.type == TokenType.ERROR:
			self.variables[self.var_pos].type = VariableType.ERROR
			self.variables[self.var_pos].error_message = "Syntax error!!\n\t Unexpected Token"
			return False

		if self.cur_token.type == expected_type:
			self.cur_token = self.get_token()
			return True

		self.error(bnf, error_value, bnf_type)
		return False

	def syntax_error(self):
		return any(var.type == VariableType.ERROR for var in self.variables[:self.var_count])

	def find_variable(self, name):
		for i in range(self.var_count - 1, -1, -1):
			if self.variables[i].name == name:
				return i
		return -1

	def error(self, bnf, error_value, bnf_type):
		self.variables[self.var_pos].type = VariableType.ERROR
		self.variables[self.var_pos].error_message = f"{bnf} expected {bnf_type} but got {error_value or 'empty string'}"
		
	def number(self):
		if self.syntax_error():
			return 0

		if self.cur_token.type != TokenType.NUMBER:
			self.variables[self.var_pos].type = VariableType.ERROR
			return 0

		try:
			value = float(self.cur_token.value)
		except ValueError:
			self.error("<number> → value is not a float", self.cur_token.value, "number")
			self.variables[self.var_pos].type = VariableType.ERROR
			return 0

		self.cur_token = self.get_token()
		return value

	def factor(self):
		if self.syntax_error():
			return 0

		if self.cur_token.type != TokenType.NUMBER:
			self.error("<factor> → <number>", self.cur_token.value, "number")
			return 0

		return self.number()

	def term(self):
		left = self.factor()
		if self.syntax_error():
			return 0

		while self.cur_token.type == TokenType.TERMOP:
			op = self.cur_token
			self.cur_token = self.get_token()

			right = self.factor()
			if self.syntax_error():
				return 0

			if op.value == "*":
				left *= right
			elif op.value == "/":
				if right == 0:
					self.error("<term> → Division by zero", "", "operation")
					return 0
				left /= right

		return left
	
	def aexpr(self):
		result = self.term()
		if self.syntax_error():
			return 0

		while self.cur_token.type == TokenType.AEXPROP:
			op = self.cur_token
			self.cur_token = self.get_token()

			right = self.term()
			if self.syntax_error():
				return 0

			if op.value == "+":
				result += right
			elif op.value == "-":
				result -= right

		return result

	def bexpr(self):
		left = self.number()
		if self.syntax_error():
			return False

		op = self.cur_token
		self.cur_token = self.get_token()
		right = self.number()
		if self.syntax_error():
			self.error("<bexpr> → <number> <relop> <number>", self.cur_token.value, "<number>")
			return False

		if op.value == "==":
			return left == right
		elif op.value == "!=":
			return left != right
		elif op.value == "<":
			return left < right
		elif op.value == ">":
			return left > right
		elif op.value == "<=":
			return left <= right
		elif op.value == ">=":
			return left >= right
		return False

	def expr(self):
		next_token = self.lookup_next_token()
		if next_token.type in {TokenType.AEXPROP, TokenType.TERMOP, TokenType.SEMI}:
			self.variables[self.var_pos].type = VariableType.NUMBER
			self.variables[self.var_pos].value = self.aexpr()
			self.var_pos += 1
			self.var_count += 1
			return
		elif next_token.type == TokenType.RELOP:
			self.variables[self.var_pos].type = VariableType.BOOLEAN
			self.variables[self.var_pos].is_true = self.bexpr()
			self.var_pos += 1
			self.var_count += 1
			return
		self.error("<expr> → <bexpr> | <aexpr>", next_token.value, "<bexpr> or <aexpr>")
		return
		
	def statement(self):
		if self.variables[self.var_pos].type == VariableType.ERROR:
			return

		if self.cur_token.type == TokenType.VAR:
			self.variables[self.var_pos].name = self.cur_token.value

			if not self.match(TokenType.VAR, "<statement> → <var> = <expr> ; | print <var> ;\n", self.cur_token.value, "<var>"):
				return

			if not self.match(TokenType.ASSIGN, "<statement> → <var> = <expr> ; | print <var> ;\n", self.cur_token.value, "="):
				return

			self.expr()

			if not self.match(TokenType.SEMI, "<statement> → <var> = <expr> ; | print <var> ;\n", self.cur_token.value, ";"):
				return
			return

		if self.cur_token.type == TokenType.PRINT:
			self.cur_token = self.get_token()

			if self.cur_token.type != TokenType.VAR:
				self.error("<statement> → <var> = <expr>\n", self.cur_token.value, "<var>")
				return

			variable_index = self.find_variable(self.cur_token.value)
			self.cur_token = self.get_token()

			if not self.match(TokenType.SEMI, "<statement> → <var> = <expr> ; | print <var>;", self.cur_token.value, ";"):
				return

			if variable_index == -1:
				print("print variable not found it's not syntax error but it's runtime error")
				return

			var = self.variables[variable_index]

		if var.type == VariableType.NUMBER:
			self.temp_buffer = f"{var.name} = {int(var.value)}\n"
			self.result_buffer += self.temp_buffer
		elif var.type == VariableType.BOOLEAN:
			self.temp_buffer = f"{var.name} = {'TRUE' if var.is_true else 'FALSE'}\n"
			self.result_buffer += self.temp_buffer
		return
		
		return
	
	def find_error_index(self):
		for i, var in enumerate(self.variables):
			if var.type == VariableType.ERROR:
				return i
		return -1

	def program(self):
		if self.input[self.pos] == '\0':
			return

		self.cur_token = self.get_token()
		while self.cur_token.type != TokenType.TERMINATE:
			if self.cur_token.type == TokenType.ERROR:
				print("Syntax error!!")
				return
			self.statement()
			if self.syntax_error():
				print("Syntax error!!")
				return
		print(self.result_buffer)
		
	def remove_space(self, s):
		s = s.replace('\n', '\0').strip()
		s = s.replace(' ', '').replace('\t', '')
		return s

	def is_valid_input(self, s):
		if s == '\0' or s == '\n':
			return False

		for char in s:
			if ord(char) == 27 or (ord(char) <= 0x1f or ord(char) == 0x7f):
				return False
		return True

	def init_static(self):
		self.pos = 0
		self.var_pos = 0
		self.var_count = 1
		self.variables = [Variable() for _ in range(64)]
		self.result_buffer = ""
		self.temp_buffer = ""
		self.cur_token = Token()

def main():
	interpreter = Interpreter()

	while True:
		temp = input(">> ")
		temp = interpreter.remove_space(temp)

		if temp == "terminate":
			break

		if not interpreter.is_valid_input(temp):
			print("Invalid input. Please try again.")
			continue

		interpreter.init_static()
		interpreter.input = temp
		interpreter.program()

if __name__ == "__main__":
	main()

