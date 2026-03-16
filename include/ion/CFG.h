#pragma once

#include "IR.h"

#include <vector>
#include <memory>

struct Block {
    int id;
    std::string label;
    std::vector<Instruction> instructions;
    std::vector<std::unique_ptr<Block>> predecessors;
    std::vector<std::unique_ptr<Block>> successors;
}

struct Function {
    std::string name;
    std::vector<std::unique_ptr<Block>> blocks;
}