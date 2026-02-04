#include "IR.h"

int main() {
    /* Target Program
    Block 1 (Label: "entry"):
        %1 = 10
        %2 = 20
        %3 = ADD %1, %2
        BEQ %3, 30, L_EXIT  <-- Branch to exit if %3 == 30

    Block 2 (Label: "L_EXIT"):
        RET %3
    */

    VReg r1 = {1};  // Virtual Register %1
    VReg r2 = {2};  // Virtual Register %2
    VReg r3 = {3};  // Virtual Register %3
    
    Imm i10 = {10}; // Literal number 10
    Imm i20 = {20}; // Literal number 20
    Imm i30 = {30}; // Literal number 30

    Label l_exit = {.name = "L_EXIT"};      // Target for jump

    Function myFunc;
    myFunc.name = "test_func";

    BasicBlock entryBlock;
    entryBlock.id = 1;
    entryBlock.label = "entry";

    BasicBlock exitBlock;
    exitBlock.id = 2;
    exitBlock.label = "L_EXIT";

    // %1 = 10 (MOV)
    entryBlock.instrs.push_back(Instruction {
        .op = Opcode::MOV,
        .def = r1,
        .uses = {i10}
    });

    // %2 = 20
    entryBlock.instrs.push_back(Instruction {
        .op = Opcode::MOV,
        .def = r2,
        .uses = {i20}
    });

    // %3 = ADD %1, %2
    entryBlock.instrs.push_back(Instruction {
        .op = Opcode::ADD,
        .def = r3,
        .uses = {r1, r2}
    });

    // BEQ %3, 30, L_EXIT
    entryBlock.instrs.push_back({
        Opcode::BEQ,
        std::monostate{},
        {r3, i30, l_exit}
    });

    // RET %3
    entryBlock.instrs.push_back({
        Opcode::RET,
        std::monostate{},
        {r3}
    });

    entryBlock.succs.push_back(&exitBlock);     // Entry -> Exit
    exitBlock.preds.push_back(&entryBlock);     // Exit <- Entry
    myFunc.blocks.push_back(entryBlock);
    myFunc.blocks.push_back(exitBlock);
    std::cout << "Successfully constructed function " << myFunc.name << "\n";
    for (const BasicBlock &b : myFunc.blocks) {
        for (const Instruction &i : b.instrs)
            i.dump();
    }
    return 0;
}