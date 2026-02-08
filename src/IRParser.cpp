#include "IRParser.h"

#include <iostream>
#include <stdexcept>


bool IRParser::match(TokenType type) {
    if (m_currentToken.type == type) {
        m_currentToken = m_lexer.next();
        return true;
    }

    return false;
}

void IRParser::consume(TokenType type, const std::string& errorMsg) {
    if (m_currentToken.type == type) {
        m_currentToken = m_lexer.next();
    } else {
        throw std::runtime_error(errorMsg + "Got: " + m_currentToken.toString());
    }
}

std::unique_ptr<Function> IRParser::parseFunction() {
    auto fn = std::make_unique<Function>();
    fn->name = "parsed_func";

    // NOTE: May want to ensure first thing is a label instead of assuming a block
    auto currentBlock = std::make_unique<BasicBlock>();
    currentBlock->id = 0;

    while (m_currentToken.type != TokenType::EndOfFile) {
        // Skip empty lines
        if (m_currentToken.type == TokenType::Newline) {
            m_currentToken = m_lexer.next();
            continue;
        }

        // Identify label
        if (m_currentToken.type == TokenType::Opcode && m_lexer.peek().type == TokenType::Colon) {
            // If the current block has instrctions, we must finish it and start a new one
            if (!currentBlock->instrs.empty() || !currentBlock->label.empty()) {
                fn->blocks.push_back(std::move(currentBlock));
                currentBlock = std::make_unique<BasicBlock>();
                currentBlock->id = fn->blocks.size();
            }

            parseLabel(*fn, *currentBlock);
        }

        else if (m_currentToken.type == TokenType::Opcode) {
            parseInstruction(*fn, *currentBlock);
        }

        else {
            std::cerr << "Unexpected token at top level: " << m_currentToken.toString() << "\n";
            m_currentToken = m_lexer.next();
        }
    }

    if (currentBlock && (!currentBlock->instrs.empty() || !currentBlock->label.empty())) {
        fn->blocks.push_back(std::move(currentBlock));
    }

    return fn;
}

void IRParser::parseLabel(Function& fn, BasicBlock& block) {
    block.label = std::string(m_currentToken.text);
    consume(TokenType::Opcode, "Expected label name");
    consume(TokenType::Colon, "Expected colon after label");

    if (m_currentToken.type == TokenType::Newline) {
        consume(TokenType::Newline, "Expected newline after label");
    }
}

void IRParser::parseInstruction(Function& fn, BasicBlock& block) {
    Instruction instr;

    std::string opText(m_currentToken.text);
    if (opText == "ADD") instr.op = Opcode::ADD;
    else if (opText == "SUB") instr.op = Opcode::SUB;
    else if (opText == "MOV") instr.op = Opcode::MOV;
    else if (opText == "RET") instr.op = Opcode::RET;
    else if (opText == "JMP") instr.op = Opcode::JMP;
    else if (opText == "BEQ") instr.op = Opcode::BEQ;
    else throw std::runtime_error("Unexpected Opcode: " + opText);

    consume(TokenType::Opcode, "Expecetd Opcode");

    bool firstOperand = true;
    while (m_currentToken.type != TokenType::Newline && m_currentToken.type != TokenType::EndOfFile) {
        // Handle comma
        if (!firstOperand) {
            if (m_currentToken.type == TokenType::Comma) {
                consume(TokenType::Comma, "Expected comma");
            }
        }

        // Parse operand
        if (m_currentToken.type == TokenType::Register) {
            // Skip %
            int id = std::stoi(std::string(m_currentToken.text.substr(1)));

            // For ADD/SUB/MOV, the first operand is the Def.
            // For BEQ/RET/STORE, all operands are Uses.
            bool isDef = firstOperand && (instr.op != Opcode::BEQ && instr.op != Opcode::RET && instr.op != Opcode::STORE && instr.op != Opcode::JMP);

            if (isDef)
                instr.def = VReg{id};
            else    
                instr.uses.push_back(VReg{id});
            consume(TokenType::Register, "Expected Register");
        }

        else if (m_currentToken.type == TokenType::Integer) {
            int val = std::stoi(std::string(m_currentToken.text));
            instr.uses.push_back(Imm{val});
            consume(TokenType::Integer, "Expected Integer");
        }

        else if (m_currentToken.type == TokenType::Opcode) {
            // Labels appear as identifiers (Opcode type) in the operand list
            instr.uses.push_back(Label{std::string(m_currentToken.text)});
            consume(TokenType::Opcode, "Expected Label Target");
        }

        firstOperand = false;
    }

    block.instrs.push_back(instr);

    if (m_currentToken.type == TokenType::Newline)
        consume(TokenType::Newline, "Expected Newline");
}