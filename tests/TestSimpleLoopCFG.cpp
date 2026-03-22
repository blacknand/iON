#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/GTestMatches.h"
#include <gtest/gtest.h>

namespace {
class SimpleLoopTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        func = new Function;
        Reader reader;
        *func = reader.BuildCFG("docs/iON_IR/SimpleLoop.ion");
    }

    static void TearDownTestSuite() {
        delete func;
        func = nullptr;
    }

    static Function* func;
};

Function* SimpleLoopTest::func = nullptr;

TEST_F(SimpleLoopTest, Test_INIT_BLOCK) {
    BasicBlock* INIT_BLOCK = func->labelToBlock["INIT_BLOCK"];

    SCOPED_TRACE(testing::Message() << "Block:\n" << *INIT_BLOCK);
    
    EXPECT_EQ(INIT_BLOCK->successors[0]->label, "main_block");
}

TEST_F(SimpleLoopTest, Test_main_block) {
    BasicBlock* main_block = func->labelToBlock["main_block"];

    EXPECT_EQ(main_block->predecessors[0]->label, "INIT_BLOCK");
    EXPECT_EQ(main_block->successors[0]->label, "BLOCK_C");
    EXPECT_EQ(main_block->successors[1]->label, "BLOCK_A");
}

TEST_F(SimpleLoopTest, Test_BLOCK_A) {
    BasicBlock* BLOCK_A = func->labelToBlock["BLOCK_A"];

    EXPECT_EQ(BLOCK_A->predecessors[0]->label, "main_block");
    EXPECT_EQ(BLOCK_A->successors[0]->label, "main_block");
}

TEST_F(SimpleLoopTest, Test_BLOCK_C) {
    BasicBlock* BLOCK_C = func->labelToBlock["BLOCK_C"];
    EXPECT_EQ(BLOCK_C->predecessors[0]->label, "main_block");
}

}