#include "utils.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENT: return "IDENT";
        case TokenType::INT_CONST: return "INT_CONST";
        case TokenType::FLOAT_CONST: return "FLOAT_CONST";
        case TokenType::STRING_CONST: return "STRING_CONST";
        
        // Palavras-chave
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_FLOAT: return "KW_FLOAT";
        case TokenType::KW_STRING: return "KW_STRING";
        case TokenType::KW_DEF: return "KW_DEF";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_PRINT: return "KW_PRINT";
        case TokenType::KW_READ: return "KW_READ";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_BREAK: return "KW_BREAK";
        case TokenType::KW_NEW: return "KW_NEW";
        case TokenType::KW_NULL: return "KW_NULL";
        
        // Operadores aritméticos
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::MOD: return "MOD";
        
        // Operadores relacionais
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::LE: return "LE";
        case TokenType::GE: return "GE";
        case TokenType::EQ: return "EQ";
        case TokenType::NEQ: return "NEQ";
        
        // Outros símbolos
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
        
        default: return "UNKNOWN";
    }
}

