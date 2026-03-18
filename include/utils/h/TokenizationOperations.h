#include <string_view>
#include <vector>
#include <format>

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