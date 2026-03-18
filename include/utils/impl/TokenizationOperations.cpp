/**
    NOTE: While iON does not tokenize operations,
    I still needed some kind of tokenizer just for efficient
    line parsing w/o the overhead of a regex. The functions
    in here serve as efficient methods of quickly extracing
    information out of lines for validation
*/

#include "utils/h/TokenizationOperations.h"

constexpr bool is_cond_branch_opcode(std::string_view op) noexcept {
    return op == "NZ" || op == "BEQ"; 
}

// Fast Rejection Detector
constexpr bool is_branch(std::string_view line) noexcept {
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string_view::npos) return false;
    std::string_view trimmed = line.substr(start);

    size_t op_end = trimmed.find_first_of(" \t,");
    std::string_view op = trimmed.substr(0, op_end);

    if (!is_cond_branch_opcode(op)) {
        return false;
    }

    // Count the tokens to ensure it's exactly a 1-reg (4 tokens) or 2-reg (5 tokens) branch
    int token_count = 0;
    size_t pos = 0;
    
    while (pos < trimmed.length()) {
        pos = trimmed.find_first_not_of(" \t,", pos);
        if (pos == std::string_view::npos) break;
        token_count++;
        pos = trimmed.find_first_of(" \t,", pos);
    }

    return token_count == 4 || token_count == 5;
}

// Zero-allocation tokenizer
std::vector<std::string_view> tokenize(std::string_view line) noexcept {
    std::vector<std::string_view> tokens;
    tokens.reserve(6); 
    size_t start = 0;
    while (start < line.length()) {
        start = line.find_first_not_of(" \t,", start);
        if (start == std::string_view::npos) break;
        size_t end = line.find_first_of(" \t,", start);
        tokens.push_back(line.substr(start, end - start));
        start = end;
    }
    return tokens;
}


BranchInstruction parse_branch(std::string_view line) noexcept {
    std::vector<std::string_view> tokens = tokenize(line);
    BranchInstruction result;

    if (tokens.empty() || !is_cond_branch_opcode(tokens[0])) {
        return result; 
    }

    // One-register branch (e.g., NZ x, label1, label2) -> 4 tokens
    if (tokens.size() == 4) {
        result.is_valid = true;
        result.opcode = tokens[0];
        result.use1 = tokens[1];
        result.label1 = tokens[2];
        result.label2 = tokens[3];
    }
    // Two-register branch (e.g., BEQ x, y, label1, label2) -> 5 tokens
    else if (tokens.size() == 5) {
        result.is_valid = true;
        result.opcode = tokens[0];
        result.use1 = tokens[1];
        result.use2 = tokens[2];
        result.label1 = tokens[3];
        result.label2 = tokens[4];
    }

    return result;
}

constexpr std::string_view to_string(OpCode op) noexcept {
    switch (op) {
        case OpCode::ADD:   return "ADD";
        case OpCode::SUB:   return "SUB";
        case OpCode::MUL:   return "MUL";
        case OpCode::LOAD:  return "LOAD";
        case OpCode::STORE: return "STORE";
        case OpCode::MOV:   return "MOV";
        case OpCode::RET:   return "RET";
        case OpCode::JMP:   return "JMP";
        case OpCode::BEQ:   return "BEQ";
        case OpCode::BZ:    return "BZ";
        case OpCode::BNZ:   return "BNZ";
        default:            return "UNKNOWN";
    }
}

constexpr OpCode string_to_opcode(std::string_view word) noexcept {
    if (word == "ADD") return OpCode::ADD;
    if (word == "SUB") return OpCode::SUB;
    if (word == "MUL") return OpCode::MUL;
    if (word == "LOAD") return OpCode::LOAD;
    if (word == "STORE") return OpCode::STORE;
    if (word == "MOV") return OpCode::MOV;
    if (word == "RET") return OpCode::RET;
    if (word == "JMP") return OpCode::JMP;
    if (word == "BEQ") return OpCode::BEQ;
    if (word == "BZ") return OpCode::BZ;
    if (word == "BNZ") return OpCode::BNZ;
    return OpCode::UNKNOWN;
}

constexpr OpCode extract_opcode(std::string_view line) noexcept {
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string_view::npos) {
        return OpCode::UNKNOWN;         // Empty or all-whitespace line
    }

    size_t end = line.find_first_of(" \t,", start);
    
    std::string_view first_word = line.substr(start, end - start);

    if (!first_word.empty() && first_word.back() == ':') {
        return OpCode::UNKNOWN; 
    }

    return string_to_opcode(first_word);
}

constexpr bool is_integer(std::string_view token) noexcept {
    if (token.empty()) return false;
    
    size_t start = 0;
    if (token[0] == '-') {
        if (token.length() == 1) return false; // Just a "-" is not an integer
        start = 1;
    }
    
    for (size_t i = start; i < token.length(); ++i) {
        if (token[i] < '0' || token[i] > '9') return false;
    }
    return true;
}

std::optional<std::string_view> extract_def(const std::vector<std::string_view>& tokens) noexcept {
    if (tokens.size() >= 2) {
        // If the first token is a known opcode...
        if (string_to_opcode(tokens[0]) != OpCode::UNKNOWN) {
            // ...and the second token is a register
            if (tokens[1].starts_with('%')) {
                return tokens[1];
            }
        }
    }
    return std::nullopt;
}

// Extract Uses (All %num and integers, ignoring the definition)
std::array<std::string_view, 2> extract_uses(const std::vector<std::string_view>& tokens) noexcept {
    std::array<std::string_view, 2> uses;
    if (tokens.empty()) return uses;

    size_t start_idx = 1; // Default: start checking after the opcode

    // If there is a definition, we must skip token[1] so it isn't counted as a use
    if (string_to_opcode(tokens[0]) != OpCode::UNKNOWN && 
        tokens.size() >= 2 && tokens[1].starts_with('%')) {
        start_idx = 2; 
    }

    // Extract remaining registers and any constant integers
    for (size_t i = start_idx; i < tokens.size(); ++i) {
        if (tokens[i].starts_with('%') || is_integer(tokens[i])) {
            uses.push_back(tokens[i]);
        }
    }
    
    return uses;
}

// Extract Target Labels (Strings w/o colon, not opcodes, not regs/ints)
std::array<std::string_view, 2> extract_target_labels(const std::vector<std::string_view>& tokens) noexcept {
    std::array<std::string_view, 2> labels;

    for (std::string_view token : tokens) {
        // Reject label definitions (ends with colon)
        if (token.back() == ':') continue;
        
        // Reject opcodes
        if (string_to_opcode(token) != OpCode::UNKNOWN) continue;
        
        // Reject registers and integers
        if (token.starts_with('%') || is_integer(token)) continue;

        // If it survived all checks, it's a label reference!
        labels.push_back(token);
    }

    return labels;
}