#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "lexer.hpp"
#include "grammar.hpp"
#include "ast.hpp"
#include <stack>
#include <vector>

class Parser
{
private:
    Lexer &lexer;
    Grammar grammar;
    Token current;
    Token previous;
    std::stack<ASTNode *> semanticStack;
    std::string lastType;
    std::vector<VarDeclNode *> tempParams;

    void advance();
    bool isTerminal(const std::string &symbol);
    bool matchTerminal(const std::string &terminal, TokenType type);
    void performAction(const std::string &action);

public:
    Parser(Lexer &lex);
    void parse();
    std::unique_ptr<ASTNode> root;
};

#endif
