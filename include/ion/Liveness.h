#pragma once

#include "IR.h"

#include <map>
#include <bitset>
#include <set>

struct LivenessInfo {
    std::map<int, std::vector<bool>> UEVar;
    std::map<int, std::vector<bool>> VarKill;
};

// Hold the results of the equations solved
struct LivenessResult {
    // NOTE: good for correctness + debugging, slow in practice
    std::map<int, std::set<int>> liveoutSet;
    std::map<int, std::set<int>> liveinSet;     // NOTE: have no idea if want

    // NOTE: fast, but with spare IDs may not be good idea
    // Indexed by block ID
    // std::vector<std::set<int>> liveoutSet;        
    // std::vector<std::set<int>> liveinSet;
};

class LivenessAnalysis {
public:
    // Gathers the initial information and stores in the internal bitsets
    // LivenessResult solveEquations(Function& fn);
    LivenessResult analyse(Function& fn);
    // NOTE: for expansion, use Boost.DynamicBitset
    // Block ID -> bitset
    // std::map<int, std::vector<bool>> UEVar;   
    // std::map<int, std::vector<bool>> VarKill; 
private:
    LivenessInfo computeUseDef(Function& fn);
};