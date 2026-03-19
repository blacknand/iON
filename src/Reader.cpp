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

Function Reader::BuildCFG(const std::string& filename) {
    findLeaders(filename);
    return func;
}

void Reader::FindLeaders(const std::string& filename) {
    InstrParser parser;
    std::ifstream file(filename);
    std::string str;
    std::string fileContents;
    while (std::getline(file, str)) {
        fileContents += str;
        fileContents.push_back('\n');
    }

    int idCounter = 0;
    auto it = fileContents.begin();

    /**
        Iterate through the files contents. When a label decleration
        is found, add instructions into the vector. When another label
        decleration is found, create and add a BasicBlock with the
        instructions inside it. The loop will then go back around and
        start on the label decleration.
    */

    for (it != fileContents.end()) {
        auto result = parser.parse(*it);
        if (!result) { ++it; continue; }
        auto& instr = *result;
        if (instr.form != ParsedInstr::LabelDef) { ++it; continue; }

        std::string_view blockLabel = instr.label;
        ++it;
        std::vector<Instruction> blockInstructions;

        while (it != fileContents.end()) {
            auto inner = parser.parse(*it);
            if (!inner) { ++it; continue; }

            if (inner->form == ParsedInstr::LabelDef) break;

            blockInstructions.push_back(Instruction{
                .op = instr.opcode,
                .def = (instr.def.has_value()) ? instr.def.value : std::nullopt,
                .operands = instr.uses,
                .labels = instr.targets
            });
            ++it;
        }

        auto block = std::make_uniqe<BasicBlock>(BasicBlock{
            .id = idCounter++,
            .label = blockLabel,
            .instructions = std::move(blockInstructions)
        });

        func.blocks.push_back(block);
        func.labelToBlock[blockLabel] = block;
    }
}

Function Reader::BuildGraph() {
    for (size_t i = 0; i < blocks.size(); i++) {
        /* 
           Create an edge between the block currently being processed,
           and the blocks that are referenced for branching within
           the instructions. Edges are created by using the predecessor
           and successor vectors of BasicBlock* inside of each BasicBlock.
        **/ 
        auto block = blocks[i];
        for (size_t j = 0; j < block.instructions.size(); j++) {
            /* Add edges between blocks for all instructions containing a conditional. */
            auto instr = block.instructions[j];

            /**
                if the instruction is of the form <opcode> <operand1>, <operand2>, <label1>, <label2>,
                then add the edges from the current block to the target blocks (labels)
            */
            if (instr.form == Form::CondBranch2) {
                auto target1 = labelToBlock[instr.targets[0]];
                auto target2 = labelToBlock[instr.targets[1]];

                block.successor.push_back(target1);
                block.successor.push_back(target2);
                
                target1->predecessors.push_back(block);
                target2->predecessors.push_back(block);
            }

            /**
                if the instruction is of the form <opcode> <operand1>, <operand2>, <label1>
                then add the edges from the current block to the target block 
            */
            else if (instr.form == Form::CondBranch1) {
                auto target1 = labelToBlock[instr.targets[0]];
                block.successor.push_back(target1);
                target1->predecessors.push_back(block);
            }

            /* If the instruction is an unconditional jump */
            else if (instr.form == Form::Jump) {
                auto target = labelToBlock[instrs.targets[0]];
                block.successor.push_back(target);
                target->predecessors.push_back(block);
            }
        }
    }
}