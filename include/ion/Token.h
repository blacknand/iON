#pragma once

#include <string_view>
#include <string>
#include <format>

enum class TokenType {
    Opcode,
    Register,
    Label,
    Integer,
    Comma,
    Colon,
    Newline,
    EndOfFile,
    Unkown
};

struct Token {
    TokenType type;
    std::string_view text;
    int line;

    std::string toString() const {
        return std::format("Token({}: '{}', line {})", (int)type, text, line);
    }
};