#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    IDENT, INT_CONST, FLOAT_CONST, STRING_CONST,
    KW_INT, KW_FLOAT, KW_STRING, KW_DEF, KW_FOR, KW_IF, KW_ELSE,
    KW_PRINT, KW_READ, KW_RETURN, KW_BREAK, KW_NEW, KW_NULL,
    PLUS, MINUS, STAR, SLASH, MOD,
    LT, GT, LE, GE, EQ, NEQ,
    ASSIGN, COMMA, SEMICOLON,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

#endif
