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

    Opcode add = Opcode::ADD;
    Opcode beq = Opcode::BEQ;
    Opcode ret = Opcode::RET;
    Opcode mov = Opcode::MOVE;
    VReg r1 = {.id = 1};
    VReg r2 = {.id = 2};
    VReg r3 = {.id = 3};
    Imm i10 = {.value = 10};
    Imm i20 = {.value = 20};
    Imm i30 = {.value = 30};
    Label l_exit = {.name = "L_EXIT"};
    Label entry = {.name = "entry"};

    // %1 = 10
    Instruction {
        .op = Opcode::MOV,
        .def = r1,
        .uses = {i10}
    };

    // %2 = 20
    Instruction {
        .op = Opcode::MOV,
        .def = r2,
        .uses = {i20}
    };

    // %3 = ADD %1, %2
    Instruction {
        .op = Opcode::ADD,
        .def = r3,
        .uses = {r1, r2}
    };

    // BEQ %3, 30, L_EXIT
    Instruction {
        .op = Opcode::BEQ,
        .def = l_exit,
        .uses = {i30}
    };

    Instruction {
        .op = Opcode::RET,
        .def = std::monostate{},
        .uses = {r3}
    };
}