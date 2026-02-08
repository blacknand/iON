#pragma once

#include "Token.h"

#include <vector>

class Lexer {
public:
    explicit Lexer(std::string_view source)
        : m_source(source), m_pos(0), m_line(1) {}
    
        Token next();
        Token peek() const;
private:
    std::string_view m_source;
    size_t m_pos;
    int m_line;

    char current() const;
    void advance();
    void skipWhitespace();

    Token makeToken(TokenType type, size_t start, size_t length);    
};