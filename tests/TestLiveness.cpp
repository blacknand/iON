#include "ion/CFG.h"
#include "ion/Liveness.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include <gtest/gtest.h>
#include <memory>

namespace {
class LivenessAnalysisTest : public ::testing::Test {
protected:
    static Function* dagFN, *simpleLoopFN, *nestedLoopFN, *diamondFN;

    static void SetUpTestSuite() {
        Reader r_dag, r_simple, r_nested, r_diamond;
        dagFN = new Function(r_dag.BuildCFG("docs/iON_IR/StraightLineDAG.ion"));
        simpleLoopFN = new Function(r_simple.BuildCFG("docs/iON_IR/SimpleLoop.ion"));
        nestedLoopFN = new Function(r_nested.BuildCFG("docs/iON_IR/NestedLoop.ion"));
        diamondFN = new Function(r_diamond.BuildCFG("docs/iON_IR/Diamond.ion"));
    }

    static void TearDownTestSuite() {
        delete dagFN;
        dagFN = nullptr;
        delete simpleLoopFN;
        simpleLoopFN = nullptr;
        delete nestedLoopFN;
        nestedLoopFN = nullptr;
        delete diamondFN;
        diamondFN = nullptr;
    }
};

Function* LivenessAnalysisTest::dagFN = nullptr;
Function* LivenessAnalysisTest::simpleLoopFN = nullptr;
Function* LivenessAnalysisTest::nestedLoopFN = nullptr;
Function* LivenessAnalysisTest::diamondFN = nullptr;

TEST_F(LivenessAnalysisTest, GatherInitialInfo_DAG) {

}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_SimpleLoop) {
    LivenessInfo li = computeUseDef(*simpleLoopFN);

    /** INIT_BLOCK
        + UEVar = {}
        + VarKill = {%1}
     */
    std::vector<bool> VarKill_INIT_BLOCK = li.VarKill[0];
    ASSERT_TRUE(VarKill_INIT_BLOCK[1]);

    /** main_block
        + UEVar = {%1}
        + VarKill = {}
     */
    std::vector<bool> UEVar_main_block  = li.UEVar[1];
    ASSERT_TRUE(UEVar_main_block[1]);

    /** BLOCK_A
        + UEVar = {%1}
        + VarKill = {%1}
     */
    std::vector<bool> VarKill_BLOCK_A = li.VarKill[2];
    std::vector<bool> UEVar_BLOCK_A  = li.UEVar[2];
    ASSERT_TRUE(VarKill_BLOCK_A[1]);
    ASSERT_TRUE(UEVar_BLOCK_A[1]);

    /** BLOCK_C
        + UEVar = {}
        + VarKill = {}
     */
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_NestedLoop) {
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_Diamond) {
}

TEST_F(LivenessAnalysisTest, Analyse_DAG) {
}

TEST_F(LivenessAnalysisTest, Analyse_SimpleLoop) {
}

TEST_F(LivenessAnalysisTest, Analyse_NestedLoop) {

}

TEST_F(LivenessAnalysisTest, Analyse_Diamond) {

}

}       // namespace