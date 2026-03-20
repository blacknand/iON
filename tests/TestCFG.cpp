#include "ion/IR.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

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

}

TEST_F(StraightLineDAGTest, Test_BLOCK_B) {

}

TEST_F(StraightLineDAGTest, Test_BLOCK_C) {

}

TEST_F(StraightLineDAGTest, Test_BLOCK_X) {

}

}   // namespace