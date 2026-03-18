#include <gtest/gtest.h>

#include <IR.h>
#include <Liveness.h>
#include <IRParser.h>
#include <CFGConstructor.h>
#include <Lexer.h>

#include <utility>

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

        Lexer dagLexer(straightLineDAGsrc);
        Lexer simpleLoopLexer(simpleLoopSrc);
        Lexer nestedLoopLexer(nestedLoopSrc);
        Lexer diamondLexer(diamondSrc);

        IRParser dagParser(dagLexer);
        IRParser simpleLoopParser(simpleLoopLexer);
        IRParser nestedLoopParser(nestedLoopLexer);
        IRParser diamondParser(diamondLexer);

        dagFN = dagParser.parseFunction();
        simpleLoopFN = simpleLoopParser.parseFunction();
        nestedLoopFN = nestedLoopParser.parseFunction();
        diamondFN = diamondParser.parseFunction();
    }
};

TEST_F(LivenessAnalysisTest, GatherInitialInfo_DAG) {
    LivenessInfo li = computeUseDef(*dagFN);

    std::vector<bool> init_block_varkill = li.VarKill[0];
    std::vector<bool> init_block_uevar = li.UEVar[0];

    ASSERT_EQ(init_block_varkill.size(), 3);

    ASSERT_TRUE(init_block_varkill[1]);
    ASSERT_TRUE(init_block_varkill[2]);

    std::vector<bool> block_b_varkill = li.VarKill[1];
    std::vector<bool> block_b_uevar = li.UEVar[1];

    ASSERT_EQ(block_b_uevar.size(), 4);

    ASSERT_TRUE(block_b_varkill[3]);

    ASSERT_TRUE(block_b_uevar[1]);
    ASSERT_TRUE(block_b_uevar[2]);

    std::vector<bool> block_c_uevar = li.UEVar[2];

    ASSERT_EQ(block_c_uevar.size(), 4);

    ASSERT_TRUE(block_c_uevar[3]);
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_SimpleLoop) {
    LivenessInfo li = computeUseDef(*simpleLoopFN);

    // INIT_BLOCK
    std::vector<bool> init_block_varkill = li.VarKill[0];

    ASSERT_EQ(init_block_varkill.size(), 2);

    ASSERT_TRUE(init_block_varkill[1]);

    // main_block
    std::vector<bool> main_block_varkill = li.VarKill[1];
    std::vector<bool> main_block_uevar = li.UEVar[1];

    ASSERT_TRUE(main_block_uevar[1]);

    // BLOCK_A
    std::vector<bool> block_a_varkill = li.VarKill[2];
    std::vector<bool> block_a_uevar = li.UEVar[2];

    ASSERT_EQ(main_block_uevar.size(), 2);

    ASSERT_TRUE(block_a_varkill[1]);
    ASSERT_TRUE(block_a_uevar[1]);

    // BLOCK_C
    std::vector<bool> block_c_varkill = li.VarKill[3];
    std::vector<bool> block_c_uevar = li.UEVar[3];

    ASSERT_EQ(block_c_uevar.size(), 2);

    ASSERT_TRUE(block_c_uevar[1]);
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_NestedLoop) {
    LivenessInfo li = computeUseDef(*nestedLoopFN);

    // INIT_BLOCK
    std::vector<bool> init_block_varkill = li.VarKill[0];

    ASSERT_EQ(init_block_varkill.size(), 3);
    ASSERT_TRUE(init_block_varkill[1]);
    ASSERT_TRUE(init_block_varkill[2]);

    // OUTER_BLOCK
    std::vector<bool> outer_block_uevar = li.UEVar[1];

    ASSERT_EQ(outer_block_uevar.size(), 2);
    ASSERT_TRUE(outer_block_uevar[1]);

    // OUTER_BODY
    std::vector<bool> outer_body_uevar = li.UEVar[2];
    std::vector<bool> outer_body_varkill = li.VarKill[2];

    ASSERT_EQ(outer_body_uevar.size(), 2);
    ASSERT_EQ(outer_body_varkill.size(), 2);
    ASSERT_TRUE(outer_body_uevar[1]);
    ASSERT_TRUE(outer_body_varkill[1]);

    // INNER_BLOCK
    std::vector<bool> inner_block_uevar = li.UEVar[3];

    ASSERT_EQ(inner_block_uevar.size(), 3);
    ASSERT_TRUE(inner_block_uevar[2]);

    // INNER_BODY
    std::vector<bool> inner_body_uevar = li.UEVar[4];
    std::vector<bool> inner_body_varkill = li.VarKill[4];

    ASSERT_EQ(inner_block_uevar.size(), 3);
    ASSERT_TRUE(inner_block_uevar[2]);
    ASSERT_TRUE(inner_body_varkill[2]);

    // RET_BLOCK
    std::vector<bool> ret_block_uevar = li.UEVar[5];
    std::vector<bool> ret_block_varkill = li.VarKill[5];

    ASSERT_EQ(ret_block_uevar.size(), 4);
    ASSERT_TRUE(ret_block_uevar[2]);
    ASSERT_TRUE(ret_block_uevar[1]);
    ASSERT_TRUE(ret_block_varkill[3]);
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_Diamond) {
    LivenessInfo li = computeUseDef(*diamondFN);

    // INIT_BLOCK
    std::vector<bool> init_block_varkill = li.VarKill[0];

    ASSERT_EQ(init_block_varkill.size(), 2);
    ASSERT_TRUE(init_block_varkill[1]);

    // MAIN_BLOCK
    std::vector<bool> outer_block_uevar = li.UEVar[1];

    ASSERT_EQ(outer_block_uevar.size(), 2);
    ASSERT_TRUE(outer_block_uevar[1]);

    // COND_1
    std::vector<bool> cond_1_varkill = li.VarKill[2];

    ASSERT_EQ(cond_1_varkill.size(), 2);
    ASSERT_TRUE(cond_1_varkill[1]);

    // COND_2
    std::vector<bool> cond_2_varkill = li.VarKill[3];

    ASSERT_EQ(cond_2_varkill.size(), 2);
    ASSERT_TRUE(cond_2_varkill[1]);

    // RET_BLOCK
    std::vector<bool> ret_block_uevar = li.UEVar[4];

    ASSERT_EQ(ret_block_uevar.size(), 2);
    ASSERT_TRUE(ret_block_uevar[1]);
}

TEST_F(LivenessAnalysisTest, Analyse_DAG) {
    LivenessAnalysis la;
    LivenessResult c = la.analyse(*dagFN);

    happyFunTime(10, 20);
}

TEST_F(LivenessAnalysisTest, Analyse_SimpleLoop) {
    LivenessAnalysis la;
    LivenessResult c = la.analyse(*dagFN);

}

TEST_F(LivenessAnalysisTest, Analyse_NestedLoop) {
    LivenessAnalysis la;
    LivenessResult c = la.analyse(*dagFN);

}

TEST_F(LivenessAnalysisTest, Analyse_Diamond) {
    LivenessAnalysis la;
    LivenessResult c = la.analyse(*dagFN);

}