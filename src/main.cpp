#include <fstream>
#include <sstream>
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: ./compiler <arquivo.convcc>\n";
        std::cerr << "Exemplo: ./compiler test/example1.convcc\n";
        return 1;
    }

    std::ifstream f(argv[1]);
    if (!f) {
        std::cerr << "Erro: não foi possível abrir o arquivo '" << argv[1] << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    std::string sourceCode = buffer.str();
    
    // Se o arquivo estiver vazio, não há nada para analisar
    if (sourceCode.empty()) {
        std::cout << "Programa sintaticamente correto!\n";
        std::cout << "\nTabela de símbolos:\n";
        std::cout << "(vazia)\n";
        return 0;
    }

    // Criar tabela de símbolos e analisador léxico
    SymbolTable symtab;
    Lexer lex(sourceCode, symtab);

    // Criar analisador sintático (LL(1))
    Parser parser(lex);

    // Executar análise sintática
    // Qualquer erro léxico ou sintático causará exit(1) dentro dos métodos
    parser.parse();

    // Se chegou aqui, o programa é sintaticamente correto
    std::cout << "\nTabela de símbolos:\n";
    symtab.print();

    return 0;
}