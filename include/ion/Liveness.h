#pragma once

#include "IR.h"

#include <map>
#include <bitset>
#include <set>

// Hold the results of the equations solved
struct LivenessResult {
    std::string functionName;
    std::map<int, std::set<int>> liveoutSet;
    std::map<int, std::set<int>> liveinSet;     // NOTE: have no idea if want
};

class LivenessAnalysis {
public:
    // Gathers the initial information and stores in the internal bitsets
    void computeUseDef(BasicBlock& block);
    LivenessResult solveEquations(Function& fn);
private:
    // NOTE: for expansion, use Boost.DynamicBitset
    // Block ID -> bitset
    // upperbound of 256 since the IR programs are very small
    std::map<int, std::bitset<256>> UEVar;   
    std::map<int, std::bitset<256>> VarKill; 
};