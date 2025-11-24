#include "lexer.hpp"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string &input, SymbolTable &symtab)
    : src(input), index(0), line(1), col(1), symbols(symtab) {}

char Lexer::peek() {
    if (index >= src.size()) return '\0';
    return src[index];
}

char Lexer::advance() {
    char c = peek();
    if (c == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
    index++;
    return c;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

Token Lexer::number() {
    int startCol = col;
    std::string value;

    bool isFloat = false;

    while (isdigit(peek())) {
        value += advance();
    }

    if (peek() == '.') {
        isFloat = true;
        value += advance();
        while (isdigit(peek())) {
            value += advance();
        }
    }

    return Token{
        isFloat ? TokenType::FLOAT_CONST : TokenType::INT_CONST,
        value, line, startCol
    };
}

Token Lexer::identifier() {
    int startCol = col;
    std::string value;

    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    // keywords
    if (value == "int") return {TokenType::KW_INT, value, line, startCol};
    if (value == "float") return {TokenType::KW_FLOAT, value, line, startCol};
    if (value == "string") return {TokenType::KW_STRING, value, line, startCol};
    if (value == "def") return {TokenType::KW_DEF, value, line, startCol};
    if (value == "for") return {TokenType::KW_FOR, value, line, startCol};
    if (value == "if") return {TokenType::KW_IF, value, line, startCol};
    if (value == "else") return {TokenType::KW_ELSE, value, line, startCol};
    if (value == "return") return {TokenType::KW_RETURN, value, line, startCol};
    if (value == "break") return {TokenType::KW_BREAK, value, line, startCol};
    if (value == "print") return {TokenType::KW_PRINT, value, line, startCol};
    if (value == "read") return {TokenType::KW_READ, value, line, startCol};
    if (value == "new") return {TokenType::KW_NEW, value, line, startCol};
    if (value == "null") return {TokenType::KW_NULL, value, line, startCol};

    // it's an identifier
    symbols.addOccurrence(value, line, startCol);

    return Token{TokenType::IDENT, value, line, startCol};
}

Token Lexer::stringLiteral() {
    int startCol = col;
    std::string value;

    advance(); // skip opening quote

    while (peek() != '"' && peek() != '\0') {
        value += advance();
    }

    if (peek() == '"') {
        advance();
        return {TokenType::STRING_CONST, value, line, startCol};
    }

    return {TokenType::ERROR, "Unterminated string", line, startCol};
}

Token Lexer::nextToken() {
    skipWhitespace();

    int startCol = col;
    char c = peek();

    if (c == '\0')
        return {TokenType::END_OF_FILE, "", line, col};

    if (isdigit(c))
        return number();

    if (isalpha(c) || c == '_')
        return identifier();

    if (c == '"')
        return stringLiteral();

    // symbols
    switch (c) {
        case '+': advance(); return {TokenType::PLUS, "+", line, startCol};
        case '-': advance(); return {TokenType::MINUS, "-", line, startCol};
        case '*': advance(); return {TokenType::STAR, "*", line, startCol};
        case '/': advance(); return {TokenType::SLASH, "/", line, startCol};
        case '%': advance(); return {TokenType::MOD, "%", line, startCol};
        case '(': advance(); return {TokenType::LPAREN, "(", line, startCol};
        case ')': advance(); return {TokenType::RPAREN, ")", line, startCol};
        case '{': advance(); return {TokenType::LBRACE, "{", line, startCol};
        case '}': advance(); return {TokenType::RBRACE, "}", line, startCol};
        case '[': advance(); return {TokenType::LBRACKET, "[", line, startCol};
        case ']': advance(); return {TokenType::RBRACKET, "]", line, startCol};
        case ',': advance(); return {TokenType::COMMA, ",", line, startCol};
        case ';': advance(); return {TokenType::SEMICOLON, ";", line, startCol};

        case '=':
            advance();
            if (peek() == '=') {
                advance();
                return {TokenType::EQ, "==", line, startCol};
            }
            return {TokenType::ASSIGN, "=", line, startCol};

        case '<':
            advance();
            if (peek() == '=') {
                advance();
                return {TokenType::LE, "<=", line, startCol};
            }
            return {TokenType::LT, "<", line, startCol};

        case '>':
            advance();
            if (peek() == '=') {
                advance();
                return {TokenType::GE, ">=", line, startCol};
            }
            return {TokenType::GT, ">", line, startCol};

        case '!':
            advance();
            if (peek() == '=') {
                advance();
                return {TokenType::NEQ, "!=", line, startCol};
            }
            return {TokenType::ERROR, "Unexpected '!'", line, startCol};
    }

    // unknown
    advance();
    return {TokenType::ERROR, std::string("Unexpected char: ") + c, line, startCol};
}