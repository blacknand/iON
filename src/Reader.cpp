#include "Reader.h"
#include "utils/h/TokenizationOperations.h"

Function Reader::BuildCFG(const std::string& filename) {
    findLeaders(filename);
    return func;
}

void Reader::FindLeaders(const std::string& filename) {
    std::ifstream file(filename);
    std::string str;
    std::string fileContents;
    while (std::getline(file, str)) {
        fileContents += str;
        fileContents.push_back('\n');
    }

    int idCounter = 0;
    for (auto it = fileContents.begin(); it != fileContents.end()) {
        if (auto label = extract_label_definition(*it)) {
            int blockSize = 1;
            // Encountered a label definition, so construct a BasicBlock
            std::vector<Instruction> blockInstructions;
            
            // Go from the current line until the end of the block to get all instructions
            std::vector<std::string_view> tokens = tokenize(*it);
            while (!is_branch(*it)) {
                Instruction instr{
                    .op = extract_opcode(*it),
                    .def = extract_def(tokens),
                    .labels = extract_target_labels(tokens),
                    .operands = extract_uses(tokens)
                };
                blockInstructions.push_back(instr);
                ++it;
                ++blockSize;
            }

            BasicBlock block{
                .id = IDCounter++,
                .label = *label,
                .instructions = blockInstructions
                /* Predecessors and successor links are handled during second pass */
            };

            func.blocks.push_back(std::make_unique<BasicBlock>(block));
            it += blockSize;
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