#pragma once

#include "IR.h"

#include <vector>
#include <memory>

struct BasicBlock {
    int id;
    std::string label;
    std::vector<Instruction> instructions;
    std::vector<BasicBlock*> predecessors;
    std::vector<BasicBlock*> successors;
}

struct Function {
    std::string name;
    std::vector<std::unique_ptr<Block>> blocks;
}