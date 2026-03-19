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
    for (auto it = fileContents.begin(); it != fileContents.end()) {
        ParsedInstr instrResult = parser.parse(*it);
        if (instrResult.has_value()) {
            ParsedInstr& instr = instrResult.value();
            if (instr.form == Form::LabelDef) {
                ++it;
                auto copy = it;
                ParsedInstr& copyInstrResult = parser.parse(*copy);
                std::vector<Instruction> blockInstructions;

                if (copyInstrResult.has_value()) {
                    // while (copyInstrResult.Form != Form::CondBranch1 || copyInstrResult.Form != Form::CondBranch1) {
                    while (copyInstrResult.form != Form::LabelDef) {
                        Instruction newInstr{
                            .op = copyInstrResult.opcode,
                            .def = (copyInstrResult.def.has_value()) ? copyInstrResult.def.value : std::nullopt,
                            .operands = copyInstrResult.uses,
                            .labels = copyInstrResult.targets
                        };
                        blockInstructions.push_back(newInstr);
                        ++copy;
                    }
                }

                BasicBlock block{
                    .id = idCounter++,
                    .label = instr.label,
                    .instructions = blockInstructions
                }

                func.blocks.push_back(std::make_unique<BasicBlock>(block));
                it += blockSize;
            }
        }
    }
}

Function Reader::BuildGraph() {
    /**
    1. No MaxStmt
    2. No Leader array
    3. iON IR uses explicit termiantor, so ordering does not matter. This,
    however, has a need for ordering and assumes each block is numbered(?)
    4. cbr == BEQ
    5. there is no jump
    6. jumpI = JMP
    7. The algorithm needs to handle RET

    for i ← 1 to MaxStmt do
        j← Leader[i] + 1
        while ( j ≤ n and opj / ∈ Leader) do
            j← j + 1
        j← j- 1
        Last[i]← j
        if opj is "BEQ x, y, target1, target2" then
            add edge from j to node for l1
            add edge from j to node for l2
        else if opj is "jumpI → l1" then
            add edge from j to node for l1
        else if opj is ""
        end
    */
}