/**
    Parser.cpp is a helper file for parsing and tokenizing the text,
    it uses similar structs as IR.h does, but it is only used for
    parsing. The parser is used by the reader, which is the entire
    frontend of iON. 
*/

#include "ion/IR.h"

#include <array>
#include <string_view>
#include <charconv>
#include <cstdint>
#include <optional>

/* Only used internally */
struct Operand {
    enum Kind : uint8_t { None, VR, Constant };
    Kind kind = None;
    int32_t value = 0;      // reg number or constant

    static Operand vr(int32_t r) { return {VR, r}; }
    static Operand imm(int32_t c) { return {Constant, c}; }
};

/* 
    Used in the parser for internal representation. Different
    from the IR structs defined in IR.h
**/
struct ParsedInstr {
    enum Form : uint8_t {
        LabelDef,               // LABEL:
        LoadImm,                // LOADI %1, 42
        Copy,                   // MOV %1, %2
        BinaryOp,               // ADD %1, %2, %3
        CondBranch1,            // NZ %1, T1, T2
        CondBranch2,            // BEQ %1, %2, T1, T2
        Jump,                   // JMP T1
        Ret                     // RET
    };

    Form form;
    std::string_view opcode;               // "ADD", "BEQ", etc.
    std::optional<VReg> def;                           // single definition
    std::array<Operands, 2> uses = {};
    uint8_t use_count = 0;
    std::array<std::optional<std::string_view>, 2> targets = {};
    uint8_t target_count = 0;
    std::string_view label;                // only for LabelDef
};

class InstrParser {
public:
    std::optional<ParsedInstr> parse(std::string_view line);
private:
    using Form = ParsedInstr::Form;
    static Form classify(std::string_view op);
    static int tokenize(std::string_view line, std::array<std::string_view, 6>& out);
    static Operand parse_operand(std::string_view tok);
    static std::string_view trim(std::string_view s);
};