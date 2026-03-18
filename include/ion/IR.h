#pragma once

#include <optional>
#include <variant>
#include <array>
#include <stdexcept>

struct VReg {
    // A VR is represented as %42
    int id;
    bool operator==(const VReg& other) const { return id == other.id; }
}

/* Helper for container of size 2 for the operands w/o overhead of dynamically sized container */
struct OpContainer {
    using OperandsVarient = std::variant</* const val */int, /* use */VReg>;

    std::array<OperandsVarient, 2> operands;
    size_t currentSize = 0;

    void push_back(const OperandsVarient& value) {
        if (currentSize >= 2) throw std::out_of_range("Operands capacity exceeded (>= 2)");
        operands[currentSize++] = value;
    }

    void pop_back() {
        if (currentSize > 0) currentSize--;
    }

    size_t size() const { return currentSize; }
    auto begin() { return operands.begin(); }
    auto end() { return operands.begin() + currentSize; }
}

struct OpCode {
    ADD, SUB, MUL,
    // AND, OR, XOR,
    LOAD, STORE, MOV
    RET, JMP,
    // CMP
    BEQ, BZ, BNZ /* BNE, BGT, BLT, BLE... */
}

struct Instruction {
    /**
    An instruction can be of 7 different forms:
        - Load immediate: one def, zero VR uses, carries a constant value
        - Copy: one def, one use
        - Binary op: one def, two uses, carries an opcode
        - One-register conditional branch: zero defs, one use, two target labels
        - Two-register conditional branch: zero defs, two uses, two target labels
        - Unconditional jump: zero defs, zero uses, one target label
        - Return: zero defs, zero uses, no targets
    */

    OpCode op;
    std::optional<VReg> def;
    std::optional<std::string> label;

    // The container represents the operands
    OpContainer container;
}