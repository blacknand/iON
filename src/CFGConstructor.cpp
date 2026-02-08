#include "CFGConstructor.h"

void CFGConstructor::dumpCFG(const Function& fn) {
    std::cout << "digraph " << fn.name << "{\n";
    for (const auto& block : fn.blocks) {
        for (auto* succ : block->succs) {
            std::cout << "  Block " << block->id << " -> Block" << succ->id << ";\n";
        }
    }
    std::cout << "}\n";
}