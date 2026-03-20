#include "ion/CFG.h"
#include "ion/Reader.h"
#include "ion/IR.h"

int main() {
    Reader reader;
    Function staightLineDAGFunc = reader.BuildCFG("StraightLineDAG.ion");
    Function nestedLoopFunc = reader.BuildCFG("NestedLoop.ion");
    Function simpleLoopFunc = reader.BuildCFG("SimpleLoop.ion");
    Function diamondFunc = reader.BuildCFG("Diamond.ion");
    return 0;
}
