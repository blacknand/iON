#include "ion/CFG.h"
#include "ion/Liveness.h"
#include "ion/CFG.h"
#include "ion/Reader.h"

#include "utils/OutputStreamHandling.h"

#include <boost/dynamic_bitset.hpp>
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
    boost::dynamic_bitset<> VarKill_INIT_BLOCK = li.VarKill[0];
    boost::dynamic_bitset<> UEVar_INIT_BLOCK = li.UEVar[0];
    ASSERT_TRUE(VarKill_INIT_BLOCK[1]);
    ASSERT_FALSE(UEVar_INIT_BLOCK[1]);

    /** main_block
        + UEVar = {%1}
        + VarKill = {}
     */
    boost::dynamic_bitset<> UEVar_main_block  = li.UEVar[1];
    boost::dynamic_bitset<> VarKill_main_block  = li.VarKill[1];
    ASSERT_TRUE(UEVar_main_block[1]);
    ASSERT_FALSE(VarKill_main_block[1]);

    /** BLOCK_A
        + UEVar = {%1}
        + VarKill = {%1}
     */
    boost::dynamic_bitset<> VarKill_BLOCK_A = li.VarKill[2];
    boost::dynamic_bitset<> UEVar_BLOCK_A  = li.UEVar[2];
    ASSERT_TRUE(VarKill_BLOCK_A[1]);
    ASSERT_TRUE(UEVar_BLOCK_A[1]);
    ASSERT_FALSE(VarKill_BLOCK_A[0]);
    ASSERT_FALSE(UEVar_BLOCK_A[0]);

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
    std::set<int> LiveIn_INIT_BLOCK = lr.liveinSet[0];
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(1));
    ASSERT_FALSE(LiveIn_INIT_BLOCK.count(1));

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
    ASSERT_FALSE(LiveOut_main_block.count(2));
    ASSERT_FALSE(LiveIn_main_block.count(2));

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
    ASSERT_FALSE(LiveOut_BLOCK_A.count(0));
    ASSERT_FALSE(LiveIn_BLOCK_A.count(0));

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
    std::set<int> LiveIn_INIT_BLOCK = lr.liveinSet[0];
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_INIT_BLOCK.count(2));
    ASSERT_FALSE(LiveIn_INIT_BLOCK.count(1));
    ASSERT_FALSE(LiveOut_INIT_BLOCK.count(0));

    /** OUTER_BLOCK
        + VarKill = {}
        + UEVar = {%1}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1, %2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_OUTER_BLOCK = lr.liveoutSet[1];
    std::set<int> LiveIn_OUTER_BLOCK = lr.liveinSet[1];
    ASSERT_TRUE(LiveOut_OUTER_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_OUTER_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_OUTER_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_OUTER_BLOCK.count(2));
    ASSERT_FALSE(LiveOut_OUTER_BLOCK.count(0));
    ASSERT_FALSE(LiveIn_OUTER_BLOCK.count(0));

    /** OUTER_BODY
        + VarKill = {%1}
        + UEVar = {%1}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_OUTER_BODY = lr.liveoutSet[2];
    std::set<int> LiveIn_OUTER_BODY = lr.liveinSet[2];
    ASSERT_TRUE(LiveOut_OUTER_BODY.count(1));
    ASSERT_TRUE(LiveOut_OUTER_BODY.count(2));
    ASSERT_FALSE(LiveIn_OUTER_BODY.count(0));
    ASSERT_FALSE(LiveOut_OUTER_BODY.count(3));

    /** INNER_BLOCK
        + VarKill = {}
        + UEVar = {%2}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1, %2}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_INNER_BLOCK = lr.liveoutSet[2];
    std::set<int> LiveIn_INNER_BLOCK = lr.liveinSet[2];
    ASSERT_TRUE(LiveOut_INNER_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_INNER_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_INNER_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_INNER_BLOCK.count(2));
    ASSERT_FALSE(LiveOut_INNER_BLOCK.count(3));
    ASSERT_FALSE(LiveIn_INNER_BLOCK.count(4));

    /** INNER_BODY
        + VarKill = {%2}
        + UEVar = {%2}
        + LiveOut = {%1, %2}
        + LiveOut ∩ ¬VarKill = {%1}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_INNER_BODY = lr.liveoutSet[2];
    std::set<int> LiveIn_INNER_BODY = lr.liveinSet[2];
    ASSERT_TRUE(LiveOut_INNER_BODY.count(1));
    ASSERT_TRUE(LiveOut_INNER_BODY.count(2));
    ASSERT_TRUE(LiveIn_INNER_BODY.count(1));
    ASSERT_TRUE(LiveIn_INNER_BODY.count(2));
    ASSERT_FALSE(LiveIn_INNER_BODY.count(0));
    ASSERT_FALSE(LiveIn_INNER_BODY.count(0));

    /** RET_BLOCK
        + VarKill = {%3}
        + UEVar = {%1, %2}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1, %2}
    */
    std::set<int> LiveOut_RET_BLOCK = lr.liveoutSet[2];
    std::set<int> LiveIn_RET_BLOCK = lr.liveinSet[2];
    ASSERT_TRUE(LiveOut_RET_BLOCK.count(1));
    ASSERT_TRUE(LiveOut_RET_BLOCK.count(2));
    ASSERT_TRUE(LiveIn_RET_BLOCK.count(1));
    ASSERT_TRUE(LiveIn_RET_BLOCK.count(2));
    ASSERT_FALSE(LiveIn_RET_BLOCK.count(3));
    ASSERT_FALSE(LiveOut_RET_BLOCK.count(3));
}

TEST_F(LivenessAnalysisTest, Analyse_Diamond) {
    LivenessResult lr = la.analyse(*diamondFN);

    /** INIT_BLOCK
        + VarKill = {%1}
        + UEVar = {}
        + LiveOut = {%1}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
    std::set<int> LiveOut_INNIT_BLOCK = lr.liveoutSet[0];
    std::set<int> LiveIn_INNIT_BLOCK = lr.liveinSet[0];
    ASSERT_TRUE(LiveOut_INNIT_BLOCK.count(1));
    ASSERT_FALSE(LiveOut_INNIT_BLOCK.count(0));
    
    /** MAIN_BLOCK
        + VarKill = {}
        + UEVar = {%1}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {%1}
    */
    std::set<int> LiveIn_MAIN_BLOCK = lr.liveinSet[1];
    ASSERT_TRUE(LiveIn_MAIN_BLOCK.count(1));

    /** COND_1
        + VarKill = {%1}
        + UEVar = {}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
    std::set<int> LiveOut_COND1 = lr.liveoutSet[2];
    std::set<int> LiveIn_COND1 = lr.liveinSet[2];
    ASSERT_FALSE(LiveOut_COND1.count(1));
    ASSERT_FALSE(LiveIn_COND1.count(1));

    /** COND_2
        + VarKill = {%1}
        + UEVar = {}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
    std::set<int> LiveOut_COND2 = lr.liveoutSet[3];
    std::set<int> LiveIn_COND2 = lr.liveinSet[3];
    ASSERT_FALSE(LiveOut_COND2.count(1));
    ASSERT_FALSE(LiveIn_COND2.count(1));

    /** RET_BLOCK
        + Can ingore
        + VarKill = {}
        + UEVar = {}
        + LiveOut = {}
        + LiveOut ∩ ¬VarKill = {}
        + UEVar ∪ (LiveOut ∩ ¬VarKill) = LiveIn = {}
    */
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new OutputCapture);
    return RUN_ALL_TESTS();
}

}       // namespace