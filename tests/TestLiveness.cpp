#include <gtest/gtest.h>
#include "CFG.h"
#include <memory>
#include <string>


class LivenessAnalysisTest : public ::testing::Test {
protected:
    std::unique_ptr<Function> dagFN, simpleLoopFN, nestedLoopFN, diamondFN;

    LivenessAnalysisTest() {
        std::string straightLineDAGsrc = R"(
        INIT_BLOCK:
            MOV %1, 10
            MOV %2, 20
            JMP BLOCK_B

        BLOCK_B:
            ADD %3, %1, %2
            BEQ %3, 30, BLOCK_C

        BLOCK_C:
            RET %3
        )";

        std::string simpleLoopSrc = R"(
        INIT_BLOCK:
            MOV %1, 0

        main_block:
            BEQ %1, 40, BLOCK_C

        BLOCK_A:
            ADD %1, %1, 1
            JMP main_block

        BLOCK_C:
            RET %1
        )";

        std::string nestedLoopSrc = R"(
        INIT_BLOCK:
            MOV %1, 0
            MOV %2, 0

        OUTER_BLOCK:
            BEQ %1, 10, INNER_BLOCK

        OUTER_BODY:
            ADD %1, %1, 1
            JMP OUTER_BLOCK

        INNER_BLOCK:
            BEQ %2, 5, RET_BLOCK

        INNER_BODY:
            ADD %2, %2, 1
            JMP INNER_BLOCK

        RET_BLOCK:
            ADD %3, %1, %2
            RET %3
        )";

        std::string diamondSrc = R"(
        INIT_BLOCK: 
            MOV %1, 5

        MAIN_BLOCK:
            BEQ %1, 5, COND_1
            JMP COND_2

        COND_1:
            MOV %1, 1
            JMP RET_BLOCK

        COND_2:
            MOV %1, -1
            JMP RET_BLOCK

        RET_BLOCK:
            RET %1
        )";
    }
};

TEST_F(LivenessAnalysisTest, GatherInitialInfo_DAG) {
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_SimpleLoop) {
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