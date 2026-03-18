#include "ion/IR.h"

#include <string_view>
#include <vector>
#include <format>
#include <array>
#include <optional>

struct BranchInstruction {
    bool is_valid = false;
    std::string_view opcode;
    std::string_view use1;
    std::string_view use2;          // Empty if it's a 1-register branch
    std::string_view label1;        // True / Target 1
    std::string_view label2;        // False / Target 2
};

constexpr bool is_cond_branch_opcode(std::string_view op);
constexpr bool is_branch(std::string_view line);
std::vector<std::string_view> tokenize(std::string_view line);
BranchInstruction parse_branch(std::string_view line);
constexpr OpCode extract_opcode(std::string_view line);
constexpr OpCode string_to_opcode(std::string_view word);
constexpr std::string_view to_string(OpCode op);
constexpr bool is_integer(std::string_view token);
constexpr OpCode extract_opcode(std::string_view line);
std::optional<std::string_view> extract_def(const std::vector<std::string_view>& tokens);
std::array<std::string_view, 2> extract_target_labels(const std::vector<std::string_view>& tokens);
std::array<std::string_view, 2> extract_uses(const std::vector<std::string_view>& tokens);