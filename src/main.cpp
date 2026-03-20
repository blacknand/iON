#include "ion/CFG.h"
#include "ion/Reader.h"
#include "ion/IR.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<path-to-file.ion>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    
    Reader reader;
    Function cfg = reader.BuildCFG(inputFile);
    return 0;
}
