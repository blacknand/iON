#pragma once
#include <vector>
#include <string>
#include <variant>
#include <iostream>

// Virtual Register (e.g., %1, %42)
struct VReg {
    int id;
    bool operator==(const VReg& other) const { return id == other.id; }
};

struct Imm { int value; }                   // Immediate value
struct Label { std::string name; }          // Label for jumps

// Operand can be any of these
using Operand = std::variant<VReg, Imm, Label>;

enum class Opcode {
    ADD, SUB, MUL,          // Arithmetic
    MOV,                    // Copy
    LOAD, STORE,            // Memory
    JMP,                    // Unconditional jump
    BEQ,                    // Branch if equal (conditional)
    RET                     // Return
};

// Structs
struct Instruction {
    Opcode op;
    std::variant<VReg, std::monostate> def;         // Destination (optional)
    std::vector<Operand> uses;                      // Sources

    void dump() const; 

    std::ostream& operator<<(std::ostream& os, Opcode op) {
        switch (op) {
            case Opcode::ADD:       return os << "ADD";
            case Opcode::SUB:       return os << "SUB";
            case Opcode::MUL:       return os << "MUL";
            case Opcode::MOV:       return os << "MOV";
            case Opcode::LOAD:      return os << "LOAD";
            case Opcode::STORE:     return os << "STORE";
            case Opcode::JMP:       return os << "JMP";
            case Opcode::BEQ:       return os << "BEQ";
            case Opcode::RET:       return os << "RET";
        }
    }
};

struct BasicBlock {
    int id;
    std::string label;                      // Label at the start of the block
    std::vector<Instruction> instrs;        
    std::vector<BasicBlock*> preds;         // Predecessors (who jumps to me)
    std::vector<BasicBlock*> succs;         // Successors (who I jump to)
};

struct Function {
    std::string name;
    std::vector<BasicBlock> blocks;         // Owner of the memory
};