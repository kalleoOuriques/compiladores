CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Iinclude

SRC = src/main.cpp src/lexer.cpp src/parser.cpp src/symbol_table.cpp src/grammar.cpp src/token.cpp src/utils.cpp
OBJ = $(SRC:.cpp=.o)

all: compiler

compiler: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJ) -o compiler

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f src/*.o compiler

test: compiler
	@echo "============================================"
	@echo "=== Teste 1: Programa Correto ==="
	@echo "============================================"
	./compiler test/test_correct.convcc
	@echo ""
	@echo "============================================"
	@echo "=== Teste 2: Erro Sintático (Esperado) ==="
	@echo "============================================"
	-./compiler test/test_syntax_error.convcc
	@echo ""
	@echo "============================================"
	@echo "=== Teste 3: Erro Léxico (Esperado) ==="
	@echo "============================================"
	-./compiler test/test_lexical_error.convcc
		@echo ""
	@echo "============================================"
	@echo "=== Teste 4: Erro Semântico (Esperado) ==="
	@echo "============================================"
	-./compiler test/test_semantic_error.convcc
