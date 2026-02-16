#include "Liveness.h"

LivenessInfo computeUseDef(Function& fn) {
    // Gather the initial information for liveness analysis,
    // each block has k operations of the form "x <- y op z".
    LivenessInfo li;
    for (const auto &block : fn.blocks) {
        // Initialise UEVar and VarKill with the block ID and all 0s
        li.UEVar.insert({block->id, {false}});
        li.VarKill.insert({block->id, {false}});
        int k = block->instrs.size();
        for (int i = 1; i < k; ++i) {
            Instruction instr = block->instrs[i];
            // Opcode op = instr.op;
            std::variant<std::monostate, VReg> def = instr.def;
            std::vector<Operand> uses = instr.uses;

            // Uses stores the registers used inside an instruction
            // where y = uses[0] and z = uses[1]
            // i.e., %3 = ADD %1, %2 = x <- y op z
            // if (uses.size() == 2 && 
            //     std::holds_alternative<VReg>(uses[1]) && 
            //     std::holds_alternative<VReg>(uses[2])) {
            //     // If use is a VReg, check in set using index
            //     VReg y = std::get<VReg>(uses[0]);
            //     VReg z = std::get<VReg>(uses[1]);

            //     // If y NOT IN VarKill(block)
            //     if (!li.VarKill[block->id][y.id])
            //         li.UEVar[block->id][y.id] = true;

            //     // If z NOT IN VarKill(block)
            //     if (!li.VarKill[block->id][z.id])
            //         li.UEVar[block->id][z.id] = true;

            //     // Add x (operand) to VarKill unconditionally
            //     // check not std::monostate first
            //     if (std::holds_alternative<VReg>(def)) {
            //         VReg x = std::get<VReg>(def);
            //         li.VarKill[block->id][x.id] = true;
            //     }

            //     // TODO: still need to handle non-generic instructions
            // }

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
    return lr;
}