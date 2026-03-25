/**
    Performs liveness analysis on the iON IR. Liveness.cpp
    is composed of two functions, computeUseDef which is responsible
    for gathering the initial information to create the UEVar and VarKill
    sets, which are then used by LivenessAnalysis::analyse to
    compute the LiveIn and LiveOut sets. The analysis is performed
    on the CFG in a RPO traversal, since the computations
    performed proogate backwards through the graph.
*/

#include "Liveness.h"

LivenessInfo computeUseDef(Function& fn) {
    /* 
        Gather the initial information for liveness analysis,
        each block has k operations of the (generic) form "x <- y op z". 
     **/
    LivenessInfo li;

    // Compute global maxID across all blocks so all vectors are uniformly sized
    int globalMaxID = 1;
    for (const auto &block : fn.blocks) {
        for (const auto& instr : block->instructions) {
            if (instr.def.has_value())
                globalMaxID = std::max(globalMaxID, instr.def->id);
            for (const auto &use : instr.operands) {
                if (auto* reg = std::get_if<VReg>(&use))
                    globalMaxID = std::max(globalMaxID, reg->id);
            }
        }
    }
    globalMaxID = std::max(globalMaxID, (int)fn.blocks.size());
    int numVars = globalMaxID + 1;

    for (const auto &block : fn.blocks) {
        // Initialise UEVar and VarKill with the block ID and all 0s
        std::vector<bool> uevar(numVars, false);
        std::vector<bool> varkill(numVars, false);
        li.UEVar.insert({block->id, uevar});
        li.VarKill.insert({block->id, varkill});
        int k = block->instructions.size();
        for (int i = 0; i < k; ++i) {
            const Instruction& instr = block->instructions[i];
            const std::optional<VReg>& def = instr.def;
            const auto& uses = instr.operands;

            /* Add register operands if not in VarKill */
            for (const auto &var : uses) {
                if (std::holds_alternative<VReg>(var)) {
                    // If var NOT IN VarKill(block)
                    VReg v = std::get<VReg>(var);
                    if (!li.VarKill[block->id][v.id])
                        li.UEVar[block->id][v.id] = true;
                }
            }

            // Add x (operand) to VarKill unconditionally
            if (def.has_value()) {
                VReg x = def.value();
                li.VarKill[block->id][x.id] = true;
            }

        }
    }
    return li;
}

LivenessResult LivenessAnalysis::analyse(Function& fn) {
    /**
        Compute the LiveIn and LiveOut sets for each block
        within the CFG (function) 
    */
    LivenessInfo li = computeUseDef(fn);
    LivenessResult lr;

    int N = fn.blocks.size();

    // Initialise sets to 0
    for (int i = 0; i < N; i++) {
        lr.liveoutSet[fn.blocks[i]->id] = {};
        lr.liveinSet[fn.blocks[i]->id] = {};
    }

    /* 
        changed is used to halt the algorithm,
        when no changes have been made to the sets
    **/
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < N; i++) {
            /* Recompute LiveOut and LiveIn */
            std::set<int> newLiveOutSet = {};

            // LiveOut(B) = S ∈ succs(B) ⋃ ​(UEVar(S) ∪ (LiveOut(S) − VarKill(S)))
            for (const auto& block : fn.blocks[i]->successors) {
                std::vector<bool> UEVar = li.UEVar[block->id];
                std::vector<bool> VarKill = li.VarKill[block->id];
                std::set<int> LiveOut = lr.liveoutSet[block->id];

                /* Set for LiveOut(S) − VarKill(S) */
                std::set<int> LiveOut_NotVarKill = {};

                /* Add LiveOut values that are not killed */
                for (const int& x : LiveOut) {
                    if (x >= (int)VarKill.size() || !VarKill[x]) {
                        LiveOut_NotVarKill.insert(x);
                    }
                }

                /* Set for UEVar(S) ∪ (LiveOut(S) − VarKill(S) */
                std::set<int> UEVar_U_LiveOut_NotVarKill = {};

                /* Add all of UEVar values into set */
                for (size_t u = 0; u < UEVar.size(); u++) {
                    if (UEVar[u]) {
                        UEVar_U_LiveOut_NotVarKill.insert(u);
                    }
                }

                // Union LiveOut - VarKill set with UEVar set
                UEVar_U_LiveOut_NotVarKill.insert(LiveOut_NotVarKill.begin(), LiveOut_NotVarKill.end());

                // Add recomputed LiveOut set into new set
                newLiveOutSet.insert(UEVar_U_LiveOut_NotVarKill.begin(), UEVar_U_LiveOut_NotVarKill.end());
            }

            // If the LiveOut set has changed, update
            if (lr.liveoutSet[fn.blocks[i]->id] != newLiveOutSet) {
                changed = true;
                lr.liveoutSet[fn.blocks[i]->id] = newLiveOutSet;
            }
        }

    }

    /* Compute LiveIn when LiveOut sets for each block, B, are solved */
    // LiveIn(B) = UEVar(B) ∪ (LiveOut(B) − VarKill(B))
    for (int i = 0; i < N; i++) {
        std::vector<bool> UEVar = li.UEVar[fn.blocks[i]->id];
        std::vector<bool> VarKill = li.VarKill[fn.blocks[i]->id];
        std::set<int> LiveOut = lr.liveoutSet[fn.blocks[i]->id];
        std::set<int> LiveOut_NotVarKill = {};
        for (const int& x : LiveOut) {
            if (x >= (int)VarKill.size() || !VarKill[x]) {
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
        UEVar_U_LiveOut_NotVarKill.insert(LiveOut_NotVarKill.begin(), LiveOut_NotVarKill.end());

        lr.liveinSet[fn.blocks[i]->id] = UEVar_U_LiveOut_NotVarKill;
    }

    return lr;
}
