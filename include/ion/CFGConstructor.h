#pragma once

#include "IR.h"

#include <map>
#include <string>
#include <set>

class CFGConstructor {
public:
    static void construct(Function& fn);
    static void dumpCFG(const Function& fn, const std::string& filename = "cfg.dot");
    static void dumpCFGWithLiveness(
        const Function& fn,
        const std::map<int, std::set<int>>& liveIn,
        const std::map<int, std::set<int>>& liveOut,
        const std::string& filename = "cfg_liveness.dot"
    );
private:
    static void addEdge(BasicBlock* from, BasicBlock* to);
    static std::string blockLabel(const BasicBlock& bb,
                                const std::set<int>* liveIn  = nullptr,
                                const std::set<int>* liveOut = nullptr);
    static std::string vregSetToString(const std::set<int>& s);
    static std::string instrToString(const Instruction& instr);
    static std::string operandToString(const Operand& op);
};