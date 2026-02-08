#include <gtest/gtest.h>
#include "Lexer.h"
#include "IRParser.h"
#include "IR.h"

// ==========================================
// 1. LEXER TESTS
// ==========================================

TEST(LexerTest, TokenizesSimpleInstruction) {
    // Input: "ADD %1, 10"
    std::string source = "ADD %1, 10\n"; 
    Lexer lexer(source);

    // 1. Expect Opcode "ADD"
    Token t1 = lexer.next();
    EXPECT_EQ(t1.type, TokenType::Opcode);
    EXPECT_EQ(t1.text, "ADD");

    // 2. Expect Register "%1"
    Token t2 = lexer.next();
    EXPECT_EQ(t2.type, TokenType::Register);
    EXPECT_EQ(t2.text, "%1");

    // 3. Expect Comma
    Token t3 = lexer.next();
    EXPECT_EQ(t3.type, TokenType::Comma);

    // 4. Expect Integer "10"
    Token t4 = lexer.next();
    EXPECT_EQ(t4.type, TokenType::Integer);
    EXPECT_EQ(t4.text, "10");

    // 5. Expect Newline
    Token t5 = lexer.next();
    EXPECT_EQ(t5.type, TokenType::Newline);
}

TEST(LexerTest, HandlesLabelsAndColons) {
    std::string source = "entry:\n";
    Lexer lexer(source);

    Token t1 = lexer.next();
    EXPECT_EQ(t1.type, TokenType::Opcode); // "entry" comes as Identifier/Opcode
    EXPECT_EQ(t1.text, "entry");

    Token t2 = lexer.next();
    EXPECT_EQ(t2.type, TokenType::Colon);
}

// ==========================================
// 2. PARSER TESTS
// ==========================================

TEST(ParserTest, ParsesSimpleFunction) {
    // A small but complete IR snippet
    std::string source = 
        "entry:\n"
        "  MOV %1, 42\n"
        "  RET %1\n";

    Lexer lexer(source);
    IRParser parser(lexer);

    auto fn = parser.parseFunction();

    // Verification
    ASSERT_NE(fn, nullptr);
    ASSERT_FALSE(fn->blocks.empty());
    
    // Check Block 0 (entry)
    BasicBlock* block = fn->blocks[0].get();
    EXPECT_EQ(block->label, "entry");
    ASSERT_EQ(block->instrs.size(), 2);

    // Check Instruction 1: MOV %1, 42
    // Def: %1, Uses: {42}
    Instruction& i1 = block->instrs[0];
    EXPECT_EQ(i1.op, Opcode::MOV);
    
    // Verify Def is %1
    ASSERT_TRUE(std::holds_alternative<VReg>(i1.def));
    EXPECT_EQ(std::get<VReg>(i1.def).id, 1);

    // Verify Use is 42
    ASSERT_EQ(i1.uses.size(), 1);
    ASSERT_TRUE(std::holds_alternative<Imm>(i1.uses[0]));
    EXPECT_EQ(std::get<Imm>(i1.uses[0]).value, 42);

    // Check Instruction 2: RET %1
    // Def: None, Uses: {%1}
    Instruction& i2 = block->instrs[1];
    EXPECT_EQ(i2.op, Opcode::RET);
    ASSERT_TRUE(std::holds_alternative<std::monostate>(i2.def)); // No definition
    ASSERT_EQ(i2.uses.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VReg>(i2.uses[0]));
    EXPECT_EQ(std::get<VReg>(i2.uses[0]).id, 1);
}