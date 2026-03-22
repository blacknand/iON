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