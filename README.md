# iON
iON is a standalone global register allocator. iON takes in an intermediate representation (IR) program with virtual registers (VRs) and produces an allocated IR -- the same representation it recieved but with every virtual register replaced with a physical register.

## iON pipeline
> I created iON using only Engineering a Compiler, 3rd ed (EaC). All of the stages within the pipeline are built by implementing the algorithms from EaC.
iON implements a standard pipeline for rewriting the IR from the original IR using the virtual registers (VRs) to an IR using physical registers (PRs). The rewritten IR code is different since it will include extra instructions for storing and loading data into and from registers as well as instructions for copy coalescing. 

```
graph TD
    A[iON IR with VRs] --> B[Create CFG]
    B --> C[Perform Liveness Analysis on CFG]
    C --> D[Create Interference Graph]
    D --> E[Colour Graph]
```

### CFG Construction
iON constructs a control-flow graph (CFG) from the input IR program, where the program constist of individually created blocks (called BasicBlock), connected to each other through explicit terminators (operations which explicitly transfer control from one block to another). You can then view the generated CFG using a Graphviz dump function.

### Liveness Analysis
Liveness analysis is performed on the generated CFG to create the sets LiveOut and LiveIn which are then used further down in the pipeline to construct live ranges.

### Interference Graph Construction
An interference graph is constructed to represent where live ranges -- which are constructed from the LiveIn and LiveOut sets --- interfere with each other. Two live ranges (LRs) interfere with each other if they are both live at the same point, belong to different register classes and the compiler cannot prove that they contain the same value. An edge is created between two nodes if the two nodes interfere.

### Graph Colouring
Graph colouring is implemented using the Chaitin-Briggs algorithm, as described in Engineering a Compiler, 3rd ed. The algorithm aims to colour the interference graph such that every node of the graph is coloured, but that no neighbouring nodes have the same colour.

## Building (macOS / Linux)

**Prerequisites:** CMake 3.20+, a C++20-capable compiler (Clang or GCC), Git (for fetching GoogleTest) and Boost.

```bash
# Configure (Debug by default)
cmake -S . -B build

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

To build in Release mode:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To skip building tests:

```bash
cmake -S . -B build -DION_BUILD_TESTS=OFF
cmake --build build
```