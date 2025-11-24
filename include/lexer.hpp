#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include "symbol_table.hpp"
#include <string>

class Lexer {
private:
    std::string src;
    size_t index;
    int line, col;
    SymbolTable &symbols;

    char peek();
    char advance();
    void skipWhitespace();
    Token number();
    Token identifier();
    Token stringLiteral();

public:
    Lexer(const std::string &input, SymbolTable &symtab);
    Token nextToken();
};

#endif
