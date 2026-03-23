#include "ion/CFG.h"
#include "ion/Liveness.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/OutputStreamHandling.h"

#include <gtest/gtest.h>
#include <memory>

namespace {
class LivenessAnalysisTest : public ::testing::Test {
protected:
    static Function* dagFN, *simpleLoopFN, *nestedLoopFN, *diamondFN;
    static LivenessAnalysis la;

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

LivenessAnalysis LivenessAnalysisTest::la;
Function* LivenessAnalysisTest::dagFN = nullptr;
Function* LivenessAnalysisTest::simpleLoopFN = nullptr;
Function* LivenessAnalysisTest::nestedLoopFN = nullptr;
Function* LivenessAnalysisTest::diamondFN = nullptr;

/**
    The UEVar and VarKill sets are calculated independent
    of the CFG shape, so only one test (IR program) is sufficient.
*/
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
        + Can ignore
        + UEVar = {}
        + VarKill = {}
     */
}

/**
    LiveIn and LiveOut sets must be tested on each IR program (shape),
    the iterative solcver propogates information across CFG edges
    and repeats until convergence.
*/
TEST_F(LivenessAnalysisTest, Analyse_DAG) {
    /** 

    */
}

TEST_F(LivenessAnalysisTest, Analyse_SimpleLoop) {
    LivenessResult lr = la.analyse(*simpleLoopFN);

    /** INIT_BLOCK
        + VarKill = {%1}
        + UEVar = {}
        + LiveOut = {%1}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */

    std::set<int> LiveOut_INIT_BLOCK = lr.liveoutSet[0];
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(1));

    /** main_block
        + VarKill = {}
        + UEVar = {%1}
        + LiveOut = {%1}
        + LiveOut ∩ ¬VarKill = {%1}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1}
    */
    std::set<int> LiveOut_main_block = lr.liveoutSet[1];
    std::set<int> LiveIn_main_block = lr.liveinSet[1];
    ASSERT_TRUE(LiveOut_main_block.count(1));
    ASSERT_TRUE(LiveIn_main_block.count(1));

    /** BLOCK_A
        + VarKill = {%1}
        + UEVar = {%1}
        + LiveOut = {%1}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1}
    */
    std::set<int> LiveOut_BLOCK_A = lr.liveoutSet[2];
    std::set<int> LiveIn_BLOCK_A = lr.liveinSet[2];
    ASSERT_TRUE(LiveOut_BLOCK_A.count(1));
    ASSERT_TRUE(LiveIn_BLOCK_A.count(1));

    /** BLOCK_C
        + Can ignore
        + VarKill = {}
        + UEVar = {}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
}

TEST_F(LivenessAnalysisTest, Analyse_NestedLoop) {
    LivenessResult lr = la.analyse(*nestedLoopFN);

    /** INIT_BLOCK
        + VarKill = {%1, %2}
        + UEVar = {}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
    std::set<int> LiveOut_INIT_BLOCK = lr.liveoutSet[0];
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(2));

    /** OUTER_BLOCK
        + VarKill = {}
        + UEVar = {%1}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1, %2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_OUTER_BLOCK = lr.liveoutSet[1];
    std::set<int> LiveIn_OUTER_BLOCK = lr.liveoutSet[1];
    ASSERT_TRUE(LiveOut_OUTER_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_OUTER_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_OUTER_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_OUTER_BLOCK.count(2));

    /** OUTER_BODY
        + VarKill = {%1}
        + UEVar = {%1}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_OUTER_BODY = lr.liveoutSet[2];
    std::set<int> LiveIn_OUTER_BODY = lr.liveoutSet[2];
    ASSERT_TRUE(LiveOut_OUTER_BODY.count(1));
    ASSERT_TRUE(LiveOut_OUTER_BODY.count(2));
    ASSERT_TRUE(LiveIn_OUTER_BODY.count(1));
    ASSERT_TRUE(LiveIn_OUTER_BODY.count(2));

    /** INNER_BLOCK
        + VarKill = {}
        + UEVar = {%2}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1, %2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_INNER_BLOCK = lr.liveoutSet[2];
    std::set<int> LiveIn_INNER_BLOCK = lr.liveoutSet[2];
    ASSERT_TRUE(LiveOut_INNER_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_INNER_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_INNER_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_INNER_BLOCK.count(2));

    /** INNER_BODY
        + VarKill = {%2}
        + UEVar = {%2}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_INNER_BODY = lr.liveoutSet[2];
    std::set<int> LiveIn_INNER_BODY = lr.liveoutSet[2];
    ASSERT_TRUE(LiveOut_INNER_BODY.count(1));
    ASSERT_TRUE(LiveOut_INNER_BODY.count(2));
    ASSERT_TRUE(LiveIn_INNER_BODY.count(1));
    ASSERT_TRUE(LiveIn_INNER_BODY.count(2));

    /** RET_BLOCK
        + VarKill = {%3}
        + UEVar = {%1, %2}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_RET_BLOCK = lr.liveoutSet[2];
    std::set<int> LiveIN_RET_BLOCK = lr.liveoutSet[2];
    ASSERT_TRUE(LiveOut_RET_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_RET_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_RET_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_RET_BLOCK.count(2));
}

TEST_F(LivenessAnalysisTest, Analyse_Diamond) {

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new OutputCapture);
    return RUN_ALL_TESTS();
}

}       // namespace