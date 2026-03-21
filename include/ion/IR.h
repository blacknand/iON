#pragma once

#include <string>
#include <optional>
#include <variant>
#include <array>
#include <stdexcept>
#include <ostream>

struct VReg {
    // A VR is represented as %42
    int id;
    bool operator==(const VReg& other) const { return id == other.id; }
};

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
};

enum class OpCode {
    ADD, SUB, MUL,
    // AND, OR, XOR,
    LOAD, STORE, MOV,
    RET, JMP,
    // CMP
    BEQ, BZ, BNZ /* BNE, BGT, BLT, BLE... */
};

using Operands = std::variant<std::monostate, /* const val */int, /* use */VReg>;
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
    // std::optional<std::string> label;
    std::array<std::optional<std::string>, 2> labels;

    // may need to use std::monostate
    std::array<Operands, 2> operands;

    // The container represents the operands
    // OpContainer container;
};

inline std::ostream& operator<<(std::ostream& os, const VReg& v) {
    return os << "%" << v.id;
}

inline std::ostream& operator<<(std::ostream& os, OpCode op) {
    switch (op) {
        case OpCode::ADD:   return os << "ADD";
        case OpCode::SUB:   return os << "SUB";
        case OpCode::MUL:   return os << "MUL";
        case OpCode::LOAD:  return os << "LOAD";
        case OpCode::STORE: return os << "STORE";
        case OpCode::MOV:   return os << "MOV";
        case OpCode::RET:   return os << "RET";
        case OpCode::JMP:   return os << "JMP";
        case OpCode::BEQ:   return os << "BEQ";
        case OpCode::BZ:    return os << "BZ";
        case OpCode::BNZ:   return os << "BNZ";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Operands& op) {
    std::visit([&os](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>)
            return;
        else
            os << val;
    }, op);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Instruction& instr) {
    // def
    if (instr.def.has_value())
        os << "def = " << instr.def.value() << "; OpCode = ";

    // opcode
    os << instr.op << "; "; // assumes OpCode has a << overload

    // operands
    bool first = true;
    for (const auto& operand : instr.operands) {
        if (std::holds_alternative<std::monostate>(operand))
            continue;
        // skip default-constructed operands (int 0 is ambiguous,
        // so you may want a std::monostate sentinel instead)
        if (!first) os << ", ";
        else os << " operands = [";
        os << operand;
        first = false;
    }
    os << "]; labels = ";

    // labels
    for (const auto& label : instr.labels) {
        if (label.has_value())
            os << " " << label.value();
    }

    return os;
}