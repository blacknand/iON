#pragma once

#include "IRParser.h"

class CFGConstructor {
public:
    void findLeaders();
    void buildGraph();
    void dumpCFG(const Function& fn);
public:
    // Some kind of graph object
};