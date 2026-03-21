#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/GTestMatches.h"
#include <gtest/gtest.h>

/**
Test:
    + Each register has the correct value
    + Each block is pointing to the correct branching block(s)
    + Each block has the correct number of instructions
    + Correct number of opcodes, uses, targets, etc. in each instruction
    + There is the correct number of edges, no extra, no less than required
    + Correct number of blocks within function
    + Parsing correct?
    + Predecessor and successors lists are correct

Considerations:
    + There could be use for a Death Test because of the pointer handling
    + There should be sufficient logging + output
    + Tests are generally the same but are run on 4 different IR programs,
      could value paramterized tests be useful here?
    + Seperate fixtures for each function, and then the suite of unit tests
      inside of each fixture all using the same function object
*/

namespace {
class StraightLineDAGTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        func = new Function;
        Reader reader;
        *func = reader.BuildCFG("docs/iON_IR/StraightLineDAG.ion");
    }

    static void TearDownTestSuite() {
        delete func;
        func = nullptr;
    }

    static Function* func;
};

Function* StraightLineDAGTest::func = nullptr;

TEST_F(StraightLineDAGTest, Test_INIT_BLOCK) {
    BasicBlock* INIT_BLOCK = func->labelToBlock["INIT_BLOCK"];

    EXPECT_EQ(INIT_BLOCK->instructions.size(), 3);

    EXPECT_EQ(INIT_BLOCK->instructions[0].op, OpCode::MOV);
    EXPECT_EQ(INIT_BLOCK->instructions[1].op, OpCode::MOV);
    EXPECT_EQ(INIT_BLOCK->instructions[2].op, OpCode::JMP);
    
    EXPECT_EQ(INIT_BLOCK->instructions[0].def.value().id, 1);
    EXPECT_EQ(INIT_BLOCK->instructions[1].def.value().id, 2);

    EXPECT_THAT(INIT_BLOCK->instructions[0].operands[0], IsOperandInt(10));
    EXPECT_THAT(INIT_BLOCK->instructions[1].operands[0], IsOperandInt(20));

    EXPECT_EQ(INIT_BLOCK->instructions[2].labels[0], "BLOCK_B");

    EXPECT_EQ(INIT_BLOCK->successors[0]->label, "BLOCK_B");
}

TEST_F(StraightLineDAGTest, Test_BLOCK_B) {
    BasicBlock* BLOCK_B = func->labelToBlock["BLOCK_B"];

    EXPECT_EQ(BLOCK_B->instructions[0].op, OpCode::ADD);
    EXPECT_EQ(BLOCK_B->instructions[1].op, OpCode::BEQ);

    EXPECT_EQ(BLOCK_B->instructions[0].def.value().id, 3);

    VReg reg1{.id=1}, reg2{.id=2}, reg3{.id=3};
    EXPECT_THAT(BLOCK_B->instructions[0].operands[0], reg1);
    EXPECT_THAT(BLOCK_B->instructions[0].operands[1], reg2);
    EXPECT_THAT(BLOCK_B->instructions[1].operands[0], reg3);
    EXPECT_THAT(BLOCK_B->instructions[1].operands[1], IsOperandInt(30));

    EXPECT_THAT(BLOCK_B->instructions[1].labels[0], "BLOCK_C");
    EXPECT_THAT(BLOCK_B->instructions[1].labels[1], "BLOCK_X");

    EXPECT_EQ(BLOCK_B->predecessors[0]->label, "INIT_BLOCK");
    EXPECT_EQ(BLOCK_B->successors[0]->label, "BLOCK_C");
    EXPECT_EQ(BLOCK_B->successors[1]->label, "BLOCK_X");
}


TEST_F(StraightLineDAGTest, Test_BLOCK_C) {
    BasicBlock* BLOCK_C = func->labelToBlock["BLOCK_C"];

    EXPECT_EQ(BLOCK_C->predecessors[0]->label, "BLOCK_B");

    EXPECT_EQ(BLOCK_C->instructions[0].op, OpCode::RET);
}

TEST_F(StraightLineDAGTest, Test_BLOCK_X) {
    BasicBlock* BLOCK_X = func->labelToBlock["BLOCK_X"];

    EXPECT_EQ(BLOCK_X->predecessors[0]->label, "BLOCK_B");

    EXPECT_EQ(BLOCK_X->instructions[0].op, OpCode::RET);
}

}   // namespace