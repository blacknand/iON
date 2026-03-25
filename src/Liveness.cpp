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
        li.UEVar.insert({block->id, boost::dynamic_bitset<>(numVars)});
        li.VarKill.insert({block->id, boost::dynamic_bitset<>(numVars)});
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
                        li.UEVar[block->id].set(v.id);
                }
            }

            // Add x (operand) to VarKill unconditionally
            if (def.has_value()) {
                VReg x = def.value();
                li.VarKill[block->id].set(x.id);
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
    size_t numVars = li.UEVar.empty() ? 1 : li.UEVar.begin()->second.size();

    std::map<int, boost::dynamic_bitset<>> liveout;
    for (int i = 0; i < N; i++)
        liveout[fn.blocks[i]->id] = boost::dynamic_bitset<>(numVars);

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < N; i++) {
            boost::dynamic_bitset<> newLiveOut(numVars);
            // LiveOut(B) = ⋃ S ∈ succs(B): UEVar(S) | (LiveOut(S) & ~VarKill(S))
            for (const auto& succ : fn.blocks[i]->successors)
                newLiveOut |= li.UEVar[succ->id] | (liveout[succ->id] & ~li.VarKill[succ->id]);

            if (liveout[fn.blocks[i]->id] != newLiveOut) {
                changed = true;
                liveout[fn.blocks[i]->id] = newLiveOut;
            }
        }
    }

    // Compute LiveIn and convert bitsets -> std::set<int> for LivenessResult
    // LiveIn(B) = UEVar(B) | (LiveOut(B) & ~VarKill(B))
    for (int i = 0; i < N; i++) {
        int id = fn.blocks[i]->id;
        boost::dynamic_bitset<> liveIn = li.UEVar[id] | (liveout[id] & ~li.VarKill[id]);

        for (size_t v = liveout[id].find_first(); v != boost::dynamic_bitset<>::npos; v = liveout[id].find_next(v))
            lr.liveoutSet[id].insert(static_cast<int>(v));
        for (size_t v = liveIn.find_first(); v != boost::dynamic_bitset<>::npos; v = liveIn.find_next(v))
            lr.liveinSet[id].insert(static_cast<int>(v));
    }

    return lr;
}
