#include <gtest/gtest.h>
#include <IR.h>
#include <Liveness.h>

#include <utility>

class LivenessAnalysisTest : public ::testing::Test {
protected:
    /*
    The test fixture operates on this test IR program:
    Block A (entry):
        %1 = MOV 10
        %2 = MOV 20
        JMP B

    Block B:
        %3 = ADD %1, %2
        BEQ %3, 30, C

    Block C (exit):
        RET %3
    */
    Function f;

    LivenessAnalysisTest() {
        f.name = "test_func";

        auto entry = std::make_unique<BasicBlock>();
        entry->id = 1;
        entry->label = "entry";

        auto B = std::make_unique<BasicBlock>();
        B->id = 2;
        B->label = "test_block_B";

        auto exit = std::make_unique<BasicBlock>();
        exit->id = 3;
        exit->label = "exit_block_C";

        // %1 = MOV 10
        Imm i10 = {10};
        VReg v1 = {1};
        entry->instrs.push_back(Instruction {
            .op = Opcode::MOV,
            .def = v1,
            .uses = {i10}
        });

        // %2 = MOV 20
        Imm i20 = {20};
        VReg v2 = {2};
        entry->instrs.push_back(Instruction {
            .op = Opcode::MOV,
            .def = v2,
            .uses = {i20}
        });

        // JMP B
        Operand b_exit = Label{.name="test_block_B"};
        entry->instrs.push_back(Instruction {
            .op = Opcode::JMP,
            .def = std::monostate{},
            .uses = {b_exit}
        });

        // %3 = ADD %1, %2
        VReg v3 = {3};
        B->instrs.push_back(Instruction {
            .op = Opcode::ADD,
            .def = v3,
            .uses = {v1, v2}
        });

        // BEQ %3, 30, C
        Imm i30 = {30};
        Operand c_exit = Label{.name="exit_block_C"};
        B->instrs.push_back(Instruction {
            .op = Opcode::BEQ,
            .def = std::monostate{},
            .uses = {v3, i30, c_exit}
        });

        // RET %3
        exit->instrs.push_back(Instruction {
            .op = Opcode::RET,
            .def = std::monostate(),
            .uses = {v3}
        });

        entry->succs.push_back(B.get());
        B->preds.push_back(entry.get());
        B->succs.push_back(exit.get());
        exit->preds.push_back(B.get());
        
        f.blocks.push_back(std::move(entry));
        f.blocks.push_back(std::move(B));
        f.blocks.push_back(std::move(exit));
    }
};

TEST_F(LivenessAnalysisTest, GatherInitialInfo) {
    LivenessInfo li = computeUseDef(f);

    // Entry Block
    std::vector<bool> UEVarBlk1 = li.UEVar[1];
    std::vector<bool> VarKillBlk1 = li.VarKill[1];

    for (const auto& var : VarKillBlk1)
        std::cout << var << " ";
    std::cout << "\n";

    ASSERT_EQ(VarKillBlk1.size(), 2);
    ASSERT_EQ(UEVarBlk1.size(), 0);

    EXPECT_TRUE(VarKillBlk1[1]);
    EXPECT_TRUE(VarKillBlk1[2]);
    EXPECT_FALSE(VarKillBlk1[0]);

    // Block B


    // Exit Block
}

TEST_F(LivenessAnalysisTest, GatherInitialInfo_NonStandardInstrs) {
    // LivenessAnalysis L;
    // LivenessResult li = L.analyse(f);
    
}

TEST_F(LivenessAnalysisTest, Analyse) {
    Function f;
}