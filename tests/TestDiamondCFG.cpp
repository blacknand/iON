#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/GTestMatches.h"
#include <gtest/gtest.h>

namespace {
class TestDiamondCFG : public testing::Test {
protected:
    static void SetUpTestSuite() {
        func = new Function;
        Reader reader;
        *func = reader.BuildCFG("docs/iON_IR/Diamond.ion");
    }

    static void TearDownTestSuite() {
        delete func;
        func = nullptr;
    }

    static Function* func;
};

Function* TestDiamondCFG::func = nullptr;

TEST_F(TestDiamondCFG, Test_INIT_BLOCK) {
    BasicBlock* INIT_BLOCK = func->labelToBlock["INIT_BLOCK"];

    SCOPED_TRACE(testing::Message() << "Block:\n" << *INIT_BLOCK);
    
    EXPECT_EQ(INIT_BLOCK->successors[0]->label, "MAIN_BLOCK");
}

TEST_F(TestDiamondCFG, Test_main_block) {
    BasicBlock* main_block = func->labelToBlock["MAIN_BLOCK"];
    EXPECT_EQ(main_block->predecessors[0]->label, "INIT_BLOCK");
    EXPECT_EQ(main_block->successors[0]->label, "COND_1");
    EXPECT_EQ(main_block->successors[1]->label, "COND_2");
}

TEST_F(TestDiamondCFG, Test_COND_1) {
    BasicBlock* COND_1 = func->labelToBlock["COND_1"];
    EXPECT_EQ(COND_1->successors[0]->label, "RET_BLOCK");
    EXPECT_EQ(COND_1->predecessors[0]->label, "MAIN_BLOCK");
}

TEST_F(TestDiamondCFG, Test_COND_2) {
    BasicBlock* COND_2 = func->labelToBlock["COND_2"];
    EXPECT_EQ(COND_2->successors[0]->label, "RET_BLOCK");
    EXPECT_EQ(COND_2->predecessors[0]->label, "MAIN_BLOCK");
}

TEST_F(TestDiamondCFG, Test_RET_BLOCK) {
    BasicBlock* RET_BLOCK = func->labelToBlock["RET_BLOCK"];
    EXPECT_EQ(RET_BLOCK->predecessors[0]->label, "COND_1");
    EXPECT_EQ(RET_BLOCK->predecessors[1]->label, "COND_2");
}

}