#pragma once

#include "IR.h"
#include "CFG.h"

#include <map>
#include <set>
#include <boost/dynamic_bitset.hpp>

struct LivenessInfo {
    // Block ID -> bitset indexed by register/variable ID
    std::map<int, boost::dynamic_bitset<>> UEVar;
    std::map<int, boost::dynamic_bitset<>> VarKill;
};

// Hold the results of the equations solved
struct LivenessResult {
    // Block ID -> set
    /* 
        NOTE: Because the CFG constructor and CFG tests 
        rely on a lookup table using the block label as the key,
        it may be a better idea to convert the sets to use
        label rather than using block ID
    **/
    std::map<int, std::set<int>> liveoutSet;
    std::map<int, std::set<int>> liveinSet;     
};

// TODO: Fix design; not great for usability
LivenessInfo computeUseDef(Function& fn);

class LivenessAnalysis {
public:
    // Gathers the initial information and stores in the internal bitsets
    LivenessResult analyse(Function& fn);
};