> A standalone {global|local|other} register allocator targeting Arm v9.
# iON
iON is a standalone {global|local|other} register allocator, iON takes the virtual registers created by the earlier stages of the compiler pipeline, and maps the virtual registers to the 31 registers within the Arm v9 ISA. Users can write iON IR programs, and run them using iON to produce the binary executable.

## iON IR
- Currently considering using a CFG with basic blocks, where each basic block uses three-address code linear IR
- concerned with whether this will be impressive enough, would using DAG inside of the basic block be a better idea?
- modern compilers such as LLVM and GCC use SSA form rather than linear IR, while SSA would be harder to build, it would be a lot more impressive. However, LLVM also uses very low level IR, and uses a single IR; it uses SSA for scalar values. **Check this**. Maybe LLVM uses SSA as the definitive IR.

## Optimisation within iON
- memory-to-memory model will enable the register allocator to produce a binary, and turns register allocation into an optimisation problem since it is trying to keep some values in registers longer than the input code did. compared to register-to-register, the input code may not be in an exectuable form.

## Data-Flow Analysis within iON

## Register Allocator
- from the abstract it looks like global register allocation with graph coloring and some extra features used in experimental compilers is most interesting