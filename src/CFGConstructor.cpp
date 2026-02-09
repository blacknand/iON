#include "CFGConstructor.h"

#include <algorithm>

// NOTE: helper, may want to move elsewhere (probably include/)
std::string getTargetLabel(const Instruction& instr) {
    // Returns the right most label in the Instruction
    for (auto it = instr.uses.rbegin(); it != instr.uses.rend(); ++it) {
        if (std::holds_alternative<Label>(*it)) {
            return std::get<Label>(*it).name;
        }
    }
    throw std::runtime_error("Branch instruction missing Label operand.");
}

void CFGConstructor::construct(Function& fn) {
    // Map the labels to the blocks
    std::map<std::string, BasicBlock*> labelMap;
    for (const auto& blockPtr : fn.blocks) {
        if (!blockPtr->label.empty()) {
            labelMap[blockPtr->label] = blockPtr.get();
        }
    }

    // Wire the edges
    for (size_t i = 0; i < fn.blocks.size(); ++i) {
        BasicBlock* current = fn.blocks[i].get();

        // Handle empty blocks
        if (current->instrs.empty()) continue;

        const Instruction& last = current->instrs.back();

        // Unconditional jump
        if (last.op == Opcode::JMP) {
            std::string targetLabel = getTargetLabel(last);
            if (labelMap.find(targetLabel) == labelMap.end())
                throw std::runtime_error("Undefined label: " + targetLabel);
            BasicBlock* target = labelMap[targetLabel];
            addEdge(current, target);
        } 
        // Conditional branch
        else if (last.op == Opcode::BEQ) {
            std::string targetLabel = getTargetLabel(last);
            if (labelMap.find(targetLabel) == labelMap.end())
                throw std::runtime_error("Undefined label: " + targetLabel);

            BasicBlock* target = labelMap[targetLabel];
            addEdge(current, target);

            // Implicit fall through (next block in list if condition not true)
            if (i + 1 < fn.blocks.size())
                addEdge(current, fn.blocks[i+1].get());
        }
        // Return
        else if (last.op == Opcode::RET) {
            // No successors
        } 
        // Implicit fallthrough
        else {
            if (i + 1 < fn.blocks.size())
                addEdge(current, fn.blocks[i+1].get());
        }
    }
}

void CFGConstructor::addEdge(BasicBlock* from, BasicBlock* to) {
    if (!from || !to) return;

    auto& edges = from->succs;
    if (std::find(edges.begin(), edges.end(), to) != edges.end()) return;     // Already connected

    from->succs.push_back(to);
    to->preds.push_back(from);
}

void CFGConstructor::dumpCFG(const Function& fn) {
    std::cout << "digraph " << fn.name << "{\n";
    for (const auto& block : fn.blocks) {
        for (auto* succ : block->succs) {
            std::cout << "  Block " << block->id << " -> Block" << succ->id << ";\n";
        }
    }
    std::cout << "}\n";
}
