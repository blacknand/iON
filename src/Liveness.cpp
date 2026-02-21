#include "Liveness.h"

LivenessInfo computeUseDef(Function& fn) {
    // Gather the initial information for liveness analysis,
    // each block has k operations of the (generic) form "x <- y op z".
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
            Instruction instr = block->instrs[i];       
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
    LivenessInfo li = computeUseDef(fn);
    LivenessResult lr;

    int N = fn.blocks.size();

    // Initialise sets to 0
    for (int i = 0; i < N; i++) {
        lr.liveoutSet[fn.blocks[i]->id] = {};
        lr.liveinSet[fn.blocks[i]->id] = {};
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < N; i++) {
            // Recompute LiveOut and LiveIn
            std::set<int> newLiveOutSet = {};

            // LiveOut(B) = S ∈ succs(B) ⋃ ​(UEVar(S) ∪ (LiveOut(S) − VarKill(S)))
            for (const auto& block : fn.blocks[i]->succs) {
                std::vector<bool> UEVar = li.UEVar[block->id];
                std::vector<bool> VarKill = li.VarKill[block->id];
                std::set<int> LiveOut = lr.liveoutSet[block->id];

                // Set for LiveOut(S) − VarKill(S)
                std::set<int> LiveOut_NotVarKill = {};
                for (const int& x : LiveOut) {
                    if (!VarKill[x]) {
                        LiveOut_NotVarKill.insert(x);
                    }
                }

                // Set for UEVar(S) ∪ (LiveOut(S) − VarKill(S)
                std::set<int> UEVar_U_LiveOut_NotVarKill = {};
                for (size_t u = 0; u < UEVar.size(); u++) {
                    if (UEVar[u]) {
                        UEVar_U_LiveOut_NotVarKill.insert(u);
                    }
                }
                
                newLiveOutSet.insert(UEVar_U_LiveOut_NotVarKill.begin(), UEVar_U_LiveOut_NotVarKill.end());
            }

            // If the LiveOut set has changed, update
            if (lr.liveoutSet[fn.blocks[i]->id] != newLiveOutSet) {
                changed = true;
                lr.liveoutSet[fn.blocks[i]->id] = newLiveOutSet;
            }
        }

    }

    // Compute LiveIn when LiveOut sets for each block, B, are solved
    // LiveIn(B) = UEVar(B) ∪ (LiveOut(B) − VarKill(B))
    for (int i = 0; i < N; i++) {
        std::vector<bool> UEVar = li.UEVar[fn.blocks[i]->id];
        std::vector<bool> VarKill = li.VarKill[fn.blocks[i]->id];
        std::set<int> LiveOut = lr.liveoutSet[fn.blocks[i]->id];
        std::set<int> LiveOut_NotVarKill = {};
        for (const int& x : LiveOut) {
            if (!VarKill[x]) {
                LiveOut_NotVarKill.insert(x);
            }
        }

        // Set for UEVar(B) ∪ (LiveOut(B) − VarKill(B)
        std::set<int> UEVar_U_LiveOut_NotVarKill = {};
        for (size_t u = 0; u < UEVar.size(); u++) {
            if (UEVar[u]) {
                UEVar_U_LiveOut_NotVarKill.insert(u);
            }
        }

        lr.liveinSet[fn.blocks[i]->id] = UEVar_U_LiveOut_NotVarKill;
    }

    return lr;
}