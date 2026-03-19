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
    for (it != fileContents.end()) {
        auto result = parser.parse(*it);
        if (!result) { ++it; continue; }
        auto& instr = *result;
        if (instr.form != ParsedInstr::LabelDef) { ++it; continue; }
        // NOTE: If this will skip any non label definition lines,
        // would it skip any instructions?

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

        func.blocks.push_back(std::make_unique<BasicBlock>(BasicBlock{
            .id = idCounter++,
            .label = blockLabel,
            .instructions = std::move(blockInstructions)
        }));
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