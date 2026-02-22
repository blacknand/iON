#include "IRParser.h"
#include "Lexer.h"
#include "CFGConstructor.h"
#include "Liveness.h"   

#include <fstream>
#include <sstream>
#include <iostream>

int main() {
    std::string src = R"(
    entry:
        MOV %1, 0
        MOV %2, 10
    loop:
        BEQ %1, %2, exit
    body:
        ADD %3, %1, 1
        JMP loop
    exit:
        RET %1
    )";

    Lexer lexer(src);
    IRParser parser(lexer);
    auto fn = parser.parseFunction();

    CFGConstructor::construct(*fn);

    CFGConstructor::dumpCFG(*fn, "cfg.dot");
    // Now run:  dot -Tpng cfg.dot -o cfg.png

    LivenessAnalysis la;
    auto liveness = la.analyse(*fn);

    CFGConstructor::dumpCFGWithLiveness(*fn, liveness.liveinSet, liveness.liveoutSet,
                                        "cfg_liveness.dot");

    std::cout << "Wrote cfg.dot and cfg_liveness.dot\n";
    return 0;
}