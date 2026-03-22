#pragma once

#include "IR.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

struct BasicBlock {
    int id;
    std::string label;
    std::vector<Instruction> instructions;

    /**
        Each block will have an array of predecessors and successors,
        several blocks could be pointing to the same successor, so they will require
        shared ownership. Raw pointers are used instead of shared_ptr because
        of the overhead incurred with using shared_ptr
    */
    std::vector<BasicBlock*> predecessors;
    std::vector<BasicBlock*> successors;
};

struct Function {
    std::string name;
    std::vector<std::unique_ptr<BasicBlock>> blocks;
    std::unordered_map<std::string, BasicBlock*> labelToBlock;
};

inline std::ostream& operator<<(std::ostream& os, const BasicBlock& block) {
    os << "BasicBlock " << block.id << " [" << block.label << "]\n";

    // Print Predecessors by ID
    os << "  Predecessors: ";
    if (block.predecessors.empty()) {
        os << "None";
    } else {
        for (size_t i = 0; i < block.predecessors.size(); ++i) {
            if (block.predecessors[i]) {
                os << block.predecessors[i]->id;
            } else {
                os << "null"; // Safety catch
            }
            if (i < block.predecessors.size() - 1) os << ", ";
        }
    }
    os << "\n";

    // Print Instructions
    os << "  Instructions:\n";
    if (block.instructions.empty()) {
        os << "    (empty)\n";
    } else {
        for (const auto& inst : block.instructions) {
            // Note: This requires an overloaded operator<< for your Instruction struct as well
            os << "    " << inst << "\n"; 
        }
    }

    // Print Successors by ID
    os << "  Successors: ";
    if (block.successors.empty()) {
        os << "None";
    } else {
        for (size_t i = 0; i < block.successors.size(); ++i) {
            if (block.successors[i]) {
                os << block.successors[i]->id;
            } else {
                os << "null"; // Safety catch
            }
            if (i < block.successors.size() - 1) os << ", ";
        }
    }
    os << "\n";

    return os;
}