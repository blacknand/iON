#include "Lexer.h"

#include <cctype>

Token Lexer::next() {
    skipWhitespace();

    if (m_pos >= m_source.length()) {
        return makeToken(TokenType::EndOfFile, m_pos, 0);
    }

    char c = current();
    size_t start = m_pos;

    // Handle newlines
    if (c == '\n') {
        advance();
        m_line++;
        return makeToken(TokenType::Newline, start, 1);
    }

    // Handle puncutation
    if (c == ',') { advance(); return makeToken(TokenType::Comma, start, 1); }
    if (c == ':') { advance(); return makeToken(TokenType::Colon, start, 1); }

    // Handle registers
    if (c == '%') {
        advance();
        while (std::isalnum(current())) advance();
        return makeToken(TokenType::Register, start, m_pos - start);
    }

    // Handle ints
    if (std::isdigit(c) || c == '-') {
        advance();
        while (std::isdigit(current())) advance();
        return makeToken(TokenType::Integer, start, m_pos - start);
    }

    if (std::isalpha(c) || c == '.' || c == '_') {
        while (std::isalnum(current()) || current() == '.' || current() == '_')
            advance();

        return makeToken(TokenType::Opcode, start, m_pos - start);
        // return makeToken(TokenType::Unkown, start, 1);
    }

    advance();
    return makeToken(TokenType::Opcode, start, m_pos - start);
}

Token Lexer::peek() const {
    Lexer lookahead = *this;
    return lookahead.next();
}

void Lexer::advance() {
    m_pos++;
}

char Lexer::current() const {
    if (m_pos >= m_source.length()) 
        return '\0';
    return m_source[m_pos];    
}

void Lexer::skipWhitespace() {
    while (m_pos < m_source.length() &&
        (current() == ' ' || current() == '\t' || current() == '\r')) {
            advance();
    }
}

Token Lexer::makeToken(TokenType type, size_t start, size_t length) {
    Token token;
    token.type = type;
    token.text = m_source.substr(start, length);
    token.line = m_line;
    return token;
}