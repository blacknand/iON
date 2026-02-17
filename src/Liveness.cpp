#include "Liveness.h"

LivenessInfo computeUseDef(Function& fn) {
    // Gather the initial information for liveness analysis,
    // each block has k operations of the form "x <- y op z".
    LivenessInfo li;
    for (const auto &block : fn.blocks) {
        // Get initial number of VRegs
        int maxID = 1;
        for (const auto& instr : block->instrs) {
            if (auto* def = std::get_if<VReg>(&instr.def)) {
                maxID = std::max(maxID, def->id);
            }

            for (const auto &use : instr.uses) {
                if (auto* reg = std::get_if<VReg>(&use)) {
                    maxID = std::max(maxID, reg->id);
                }
            }
        }
        int numVars = maxID + 1;

        // Initialise UEVar and VarKill with the block ID and all 0s
        std::vector<bool> uevar(numVars, false);
        std::vector<bool> varkill(numVars, false);
        li.UEVar.insert({block->id, uevar});
        li.VarKill.insert({block->id, varkill});
        int k = block->instrs.size();
        for (int i = 0; i < k; ++i) {
            Instruction instr = block->instrs[i];       // here
            // Opcode op = instr.op;
            std::variant<std::monostate, VReg> def = instr.def;
            std::vector<Operand> uses = instr.uses;

            for (const auto &var : uses) {
                if (std::holds_alternative<VReg>(var)) {
                    // If var NOT IN VarKill(block)
                    VReg v = std::get<VReg>(var);
                    if (!li.VarKill[block->id][v.id])
                        li.UEVar[block->id][v.id] = true;
                }
            }

            // Add x (operand) to VarKill unconditionally
            // check not std::monostate first
            if (std::holds_alternative<VReg>(def)) {
                VReg x = std::get<VReg>(def);
                li.VarKill[block->id][x.id] = true;
            }

        }
    }
    return li;
}

LivenessResult LivenessAnalysis::analyse(Function& fn) {
    LivenessInfo livenessInfo = computeUseDef(fn);
    LivenessResult lr;

    int N = fn.blocks.size();
    for (int i = 0; i < N; i++) {
        lr.liveoutSet[fn.blocks[i]->id] = {};
        lr.liveinSet[fn.blocks[i]->id] = {};
    }

    bool changed = true;
    int liveOut = 0, liveIn = 0;
    std::map<int, std::set<int>> prevLiveoutSet = {};
    std::map<int, std::set<int>> prevLiveoutSet = {};
    while (changed) {
        changed = false;
        for (int i = 0; i < N; i++) {
            // LiveOut(n) = UNION_ALL(m in succ(n)) { UEVar(m) UNION (LiveOut(m) MINUS VarKill(m)) }
            // LiveIn(n) = UEVar(n) UNION (LiveOut(n) MINUS VarKill(n))
            std::vector<BasicBlock*> succs = fn.blocks[i]->succs;
            // std::vector<bool> UEVar = lr.UEVar[i];
            // std::vector<bool> VarKill = lr.UEVar[i];
            int UEVar = lr.UEVar[i].size();
            int VarKill = lr.UEVar[i].size();

            lr.liveoutSet[fn.blocks[i]->id].insert();
            lr.liveinSet[fn.blocks[i]->id];
            // If current LiveOut set is different that previos
            if (prevLiveoutSet[fn.blocks[i]->id] != lr.liveoutSet[fn.blocks[i]->id]) {
                changed = true;
                prevLiveoutSet = lr.liveoutSet[fn.blocks[i]->id];
            }
        }
    }

    return lr;
}