#pragma once

#include "IR.h"

#include <vector>
#include <memory>

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
}

struct Function {
    std::string name
    std::vector<std::unique_ptr<Block>> blocks;
}