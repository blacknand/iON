#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/GTestMatches.h"
#include <gtest/gtest.h>

namespace {
class NestedLoopTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        func = new Function;
        Reader reader;
        *func = reader.BuildCFG("docs/iON_IR/NestedLoop.ion");
    }

    static void TearDownTestSuite() {
        delete func;
        func = nullptr;
    }

    static Function* func;
};

Function* NestedLoopTest::func = nullptr;

TEST_F(NestedLoopTest, Test_INIT_BLOCK) {
    BasicBlock* INIT_BLOCK = func->labelToBlock["INIT_BLOCK"];

    EXPECT_EQ(INIT_BLOCK->successors[0]->label, "OUTER_BLOCK"); 
}

TEST_F(NestedLoopTest, Test_OUTER_BLOCK) {
    BasicBlock* OUTER_BLOCK = func->labelToBlock["OUTER_BLOCK"];

    EXPECT_EQ(OUTER_BLOCK->successors[0]->label, "INNER_BLOCK"); 
    EXPECT_EQ(OUTER_BLOCK->successors[1]->label, "OUTER_BODY"); 
}

TEST_F(NestedLoopTest, Test_OUTER_BODY) {
    BasicBlock* OUTER_BODY = func->labelToBlock["OUTER_BODY"];

    EXPECT_EQ(OUTER_BODY->predecessors[0]->label, "OUTER_BLOCK");
    EXPECT_EQ(OUTER_BODY->successors[0]->label, "OUTER_BLOCK");
}

TEST_F(NestedLoopTest, Test_INNER_BLOCK) {
    BasicBlock* INNER_BLOCK = func->labelToBlock["INNER_BLOCK"];

    EXPECT_EQ(INNER_BLOCK->successors[0]->label, "RET_BLOCK");
    EXPECT_EQ(INNER_BLOCK->successors[1]->label, "INNER_BODY");
    EXPECT_EQ(INNER_BLOCK->predecessors[0]->label, "OUTER_BLOCK");
}

TEST_F(NestedLoopTest, Test_RET_BLOCK) {
    BasicBlock* RET_BLOCK = func->labelToBlock["RET_BLOCK"];

    EXPECT_EQ(RET_BLOCK->predecessors[0]->label, "INNER_BLOCK");
}

}