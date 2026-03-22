#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/GTestMatches.h"
#include <gtest/gtest.h>

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