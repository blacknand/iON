#pragma once

#include <optional>
#include <variant>

struct VReg {
    // A VR is represented as %42
    int id;
}

struct Label {
    std::string title;
}

struct OpCode {
    ADD, SUB, MUL,
    // AND, OR, XOR,
    LOAD, STORE, MOV
    RET, JMP,
    // CMP
}

struct Instruction {
    /**
    An instruction can be of 7 different forms:
        - Load immediate: one def, zero VR uses, carries a constant value
        - Copy: one def, one use
        - Binary op: one def, two uses, carries an opcode
        - One-register conditional branch: zero defs, one use, one target label
        - Two-register conditional branch: zero defs, two uses, one target label
        - Unconditional jump: zero defs, zero uses, one target label
        - Return: zero defs, zero uses, no targets
    */
    std::optinal<OpCode> op;
    std::optional<VReg> def;
    std::variant<std::monostate, /* const val */int, /* use */VReg> operands;
}