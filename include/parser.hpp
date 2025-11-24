#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "lexer.hpp"
#include "grammar.hpp"
#include <stack>

class Parser {
private:
    Lexer &lexer;
    Grammar grammar;
    Token current;

    void advance();
    bool isTerminal(const std::string &symbol);
    bool matchTerminal(const std::string &terminal, TokenType type);

public:
    Parser(Lexer &lex);
    void parse();
};

#endif
