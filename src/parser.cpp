#include "parser.hpp"
#include "utils.hpp"
#include <iostream>

Parser::Parser(Lexer &lex) : lexer(lex), grammar() {
    advance();
}

void Parser::advance() {
    current = lexer.nextToken();
    // Detectar erro léxico imediatamente
    if (current.type == TokenType::ERROR) {
        std::cerr << "Erro léxico: " << current.lexeme 
                  << " na linha " << current.line 
                  << " coluna " << current.column << "\n";
        exit(1);
    }
}

void Parser::parse() {
    std::stack<std::string> st;
    st.push("PROGRAM");

    while (!st.empty()) {
        std::string top = st.top();
        st.pop();

        // Ignorar produções vazias (ε)
        if (top == "") continue;

        // Verificar se é um terminal
        if (isTerminal(top)) {
            // Comparar terminal esperado com token atual
            if (matchTerminal(top, current.type)) {
                advance();
                continue;
            }
            
            // Erro: terminal esperado não corresponde ao token atual
            std::cerr << "Erro sintático: esperado '" << top
                      << "' mas encontrado '" << current.lexeme
                      << "' na linha " << current.line
                      << ", coluna " << current.column << "\n";
            exit(1);
        }

        // É um não-terminal: consultar tabela LL(1)
        std::string tokName = tokenTypeToString(current.type);

        auto key = std::make_pair(top, tokName);
        
        if (grammar.ll1table.count(key) == 0) {
            std::cerr << "Erro sintático: não há produção para (" << top 
                      << ", " << tokName << ")\n";
            std::cerr << "Token inesperado '" << current.lexeme
                      << "' na linha " << current.line 
                      << ", coluna " << current.column << "\n";
            exit(1);
        }

        // Empilhar produção na ordem reversa
        auto production = grammar.ll1table[key];
        for (int i = (int)production.size() - 1; i >= 0; --i) {
            st.push(production[i]);
        }
    }

    std::cout << "Programa sintaticamente correto!\n";
}

bool Parser::isTerminal(const std::string &symbol) {
    // Todos os terminais são tokens ou símbolos especiais
    if (symbol == "IDENT" || symbol == "INT_CONST" || symbol == "FLOAT_CONST" || 
        symbol == "STRING_CONST" || symbol == "END_OF_FILE") {
        return true;
    }
    
    // Palavras-chave
    if (symbol.rfind("KW_", 0) == 0) {
        return true;
    }
    
    // Operadores e símbolos
    if (symbol == "PLUS" || symbol == "MINUS" || symbol == "STAR" || 
        symbol == "SLASH" || symbol == "MOD" ||
        symbol == "LT" || symbol == "GT" || symbol == "LE" || 
        symbol == "GE" || symbol == "EQ" || symbol == "NEQ" ||
        symbol == "ASSIGN" || symbol == "COMMA" || symbol == "SEMICOLON" ||
        symbol == "LPAREN" || symbol == "RPAREN" || 
        symbol == "LBRACE" || symbol == "RBRACE" ||
        symbol == "LBRACKET" || symbol == "RBRACKET") {
        return true;
    }
    
    return false;
}

bool Parser::matchTerminal(const std::string &terminal, TokenType type) {
    if (terminal == "IDENT") return type == TokenType::IDENT;
    if (terminal == "INT_CONST") return type == TokenType::INT_CONST;
    if (terminal == "FLOAT_CONST") return type == TokenType::FLOAT_CONST;
    if (terminal == "STRING_CONST") return type == TokenType::STRING_CONST;
    if (terminal == "END_OF_FILE") return type == TokenType::END_OF_FILE;
    
    // Palavras-chave
    if (terminal == "KW_INT") return type == TokenType::KW_INT;
    if (terminal == "KW_FLOAT") return type == TokenType::KW_FLOAT;
    if (terminal == "KW_STRING") return type == TokenType::KW_STRING;
    if (terminal == "KW_DEF") return type == TokenType::KW_DEF;
    if (terminal == "KW_FOR") return type == TokenType::KW_FOR;
    if (terminal == "KW_IF") return type == TokenType::KW_IF;
    if (terminal == "KW_ELSE") return type == TokenType::KW_ELSE;
    if (terminal == "KW_PRINT") return type == TokenType::KW_PRINT;
    if (terminal == "KW_READ") return type == TokenType::KW_READ;
    if (terminal == "KW_RETURN") return type == TokenType::KW_RETURN;
    if (terminal == "KW_BREAK") return type == TokenType::KW_BREAK;
    if (terminal == "KW_NEW") return type == TokenType::KW_NEW;
    if (terminal == "KW_NULL") return type == TokenType::KW_NULL;
    
    // Operadores e símbolos
    if (terminal == "PLUS") return type == TokenType::PLUS;
    if (terminal == "MINUS") return type == TokenType::MINUS;
    if (terminal == "STAR") return type == TokenType::STAR;
    if (terminal == "SLASH") return type == TokenType::SLASH;
    if (terminal == "MOD") return type == TokenType::MOD;
    if (terminal == "LT") return type == TokenType::LT;
    if (terminal == "GT") return type == TokenType::GT;
    if (terminal == "LE") return type == TokenType::LE;
    if (terminal == "GE") return type == TokenType::GE;
    if (terminal == "EQ") return type == TokenType::EQ;
    if (terminal == "NEQ") return type == TokenType::NEQ;
    if (terminal == "ASSIGN") return type == TokenType::ASSIGN;
    if (terminal == "COMMA") return type == TokenType::COMMA;
    if (terminal == "SEMICOLON") return type == TokenType::SEMICOLON;
    if (terminal == "LPAREN") return type == TokenType::LPAREN;
    if (terminal == "RPAREN") return type == TokenType::RPAREN;
    if (terminal == "LBRACE") return type == TokenType::LBRACE;
    if (terminal == "RBRACE") return type == TokenType::RBRACE;
    if (terminal == "LBRACKET") return type == TokenType::LBRACKET;
    if (terminal == "RBRACKET") return type == TokenType::RBRACKET;
    
    return false;
}
