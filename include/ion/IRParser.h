#pragma once

#include "IR.h"
#include "Lexer.h"

#include <string>
#include <memory>
#include <map>

class IRParser {
public:
    explicit Parser(Lexer& lexer) : m_lexer(lexer) {
        m_currentToken = m_lexer.next();
    }

    std::unique_ptr<Function> parseFunction();
private:
    Lexer& m_lexer;
    Token m_currentToken;

    void parseLine(Function& fn);
    void parseLabel(Function& fn);
    void parseInstruction(Function& fn);

    void consume(TokenType type, const std::string& errorMsg);
    bool match(TokenType type);

    std::map<std::string, int> m_labelMap;
};