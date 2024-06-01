# Define the compiler and the bin directory
JAVAC = javac
JAVA = java
BIN = ./bin
SRC = ./Java_Assignment

# Define the main class
MAIN = Java_Assignment.Main

# Define default make target
all: compile run

# Compile the Java program
compile:
	mkdir -p $(BIN)
	$(JAVAC) -d $(BIN) ./Main.java

# Run the Java program
run:
	$(JAVA) -cp $(BIN) $(MAIN)

# Clean up binary files
clean:
	rm -rf $(BIN)