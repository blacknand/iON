#include "Reader.h"

Function buildCFG(const std::string& filename) {
    findLeaders(filename);
}

void Reader::findLeaders(const std::string& filename) {
    std::ifstream file(filename);
    std::string str;
    std::string fileContents;
    while (std::getline(file, str)) {
        fileContents += str;
        fileContents.push_back('\n');
    }

    for (const std::string& line : fileContents) {
        if contains_label_fast(line, ...) 
    }
}

Function Reader::buildGraph() {
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