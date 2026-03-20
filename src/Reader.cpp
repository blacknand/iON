/**
    Reader.cpp implements the CFG. It uses Parser for extracting
    all of the fields out of the input source lines. I used a unique
    reader to parse the input source file, and build the CFG
    because iON does not have a typical frontend like a full
    compiler toolchain would.

    The reader implements the CFG in a two pass manner, it first
    iterates through the source file and builds the BasicBlocks
    and then connects the blocks together on the second pass.
*/

#include "Reader.h"
#include "utils/h/Parser.h"

#include <stdexcept>
#include <fstream>
#include <string>
#include <iostream>

static OpCode toOpCode(std::string_view sv) {
    if (sv == "ADD")   return OpCode::ADD;
    if (sv == "SUB")   return OpCode::SUB;
    if (sv == "MUL")   return OpCode::MUL;
    if (sv == "MOV")   return OpCode::MOV;
    if (sv == "LOAD")  return OpCode::LOAD;
    if (sv == "STORE") return OpCode::STORE;
    if (sv == "RET")   return OpCode::RET;
    if (sv == "JMP")   return OpCode::JMP;
    if (sv == "BEQ")   return OpCode::BEQ;
    if (sv == "BZ")    return OpCode::BZ;
    if (sv == "BNZ")   return OpCode::BNZ;
    throw std::invalid_argument(std::string("Unknown opcode: ") + std::string(sv));
}

void TraverseCFG(const Function& func, const std::string& dotFileName) {
    /* Traverse the CFG and output to a .dot file for a graphiz dump */
    std::ofstream dotFile(dotFileName);
    if (!dotFile.is_open()) {
        std::cerr << "[ERROR] Could not open .dot file\n";
        return;
    }

    dotFile << "digraph CFG {\n";
    dotFile << "    node [shape=box];\n\n";
    for (const auto& blockPtr : func.blocks) {
        BasicBlock* curBlock = blockPtr.get();
        dotFile << "    \"" << curBlock << "\" [label=\"" << curBlock->label << "\"];\n";

        for (BasicBlock* connectedBlock : curBlock->successors) {
            dotFile << "    \"" << curBlock << "\" -> \"" << connectedBlock << "\";\n";
        }

        for (BasicBlock* connectedBlock : curBlock->predecessors) {
            dotFile << "    \"" << curBlock << "\" -> \"" << connectedBlock << "\";\n";
        }
    }

    dotFile << "}\n";
    std::cout << "[INFO] CFG exported to " << dotFileName << "\n";
}

Function Reader::BuildCFG(const std::string& filename) {
    FindLeaders(filename);
    BuildGraph();
    // NOTE: There is a copy here
    TraverseCFG(func, "CFG.dot");
    return std::move(func);
}

void Reader::FindLeaders(const std::string& filename) {
    /**
        Iterate through the file's lines. When a label declaration
        is found, collect subsequent instructions into a vector. When
        another label declaration is found, create and add a BasicBlock
        with the accumulated instructions. The loop then processes the
        new label on the next iteration.
    */

    InstrParser parser;
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string str;
    while (std::getline(file, str)) {
        lines.push_back(str);
    }

    int idCounter = 0;
    size_t i = 0;

    while (i < lines.size()) {
        auto result = parser.parse(lines[i]);
        if (!result || result->form != ParsedInstr::LabelDef) { ++i; continue; }

        std::string blockLabel{result->label};
        ++i;
        std::vector<Instruction> blockInstructions;

        while (i < lines.size()) {
            auto inner = parser.parse(lines[i]);
            if (!inner) { ++i; continue; }
            if (inner->form == ParsedInstr::LabelDef) break;

            // Convert string_view targets to optional<string> labels
            std::array<std::optional<std::string>, 2> labels;
            labels[0] = inner->targets[0].has_value()
                ? std::optional<std::string>{std::string(inner->targets[0].value())}
                : std::nullopt;
            labels[1] = inner->targets[1].has_value()
                ? std::optional<std::string>{std::string(inner->targets[1].value())}
                : std::nullopt;

            blockInstructions.push_back(Instruction{
                .op       = toOpCode(inner->opcode),
                .def      = inner->def,
                .operands = inner->uses,
                .labels   = labels
            });
            ++i;
        }

        auto block = std::make_unique<BasicBlock>(BasicBlock{
            .id           = idCounter++,
            .label        = blockLabel,
            .instructions = std::move(blockInstructions)
        });

        func.labelToBlock[blockLabel] = block.get();
        func.blocks.push_back(std::move(block));
    }
}

void Reader::BuildGraph() {
    /*
        Create an edge between the block currently being processed,
        and the blocks that are referenced for branching within
        the instructions. Edges are created by using the predecessor
        and successor vectors of BasicBlock* inside of each BasicBlock.
    */
    for (size_t i = 0; i < func.blocks.size(); i++) {
        BasicBlock* block = func.blocks[i].get();
        for (size_t j = 0; j < block->instructions.size(); j++) {
            const Instruction& instr = block->instructions[j];

            /**
                BEQ: <opcode> <operand1>, <operand2>, <label1>, <label2>
                Add edges from current block to both target blocks.
            */
            if (instr.op == OpCode::BEQ) {
                BasicBlock* target1 = func.labelToBlock[instr.labels[0].value()];
                BasicBlock* target2 = func.labelToBlock[instr.labels[1].value()];

                block->successors.push_back(target1);
                block->successors.push_back(target2);

                target1->predecessors.push_back(block);
                target2->predecessors.push_back(block);
            }

            /**
                BZ/BNZ: <opcode> <operand1>, <label1>, <label2>
                Add edge from current block to one target block.
            */
            else if (instr.op == OpCode::BZ || instr.op == OpCode::BNZ) {
                BasicBlock* target1 = func.labelToBlock[instr.labels[0].value()];
                block->successors.push_back(target1);
                target1->predecessors.push_back(block);
            }

            /* Unconditional jump */
            else if (instr.op == OpCode::JMP) {
                BasicBlock* target = func.labelToBlock[instr.labels[0].value()];
                block->successors.push_back(target);
                target->predecessors.push_back(block);
            }
        }
    }
}
