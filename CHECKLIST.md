# Phase 1: The Skeleton & IR (Weeks 1-2)

- [x] **1.1 Define the IR Structure**
  - Create Instruction struct (Opcode, Def, Uses).
  - Create BasicBlock struct (List of Instructions, Predecessors, Successors).
  - Create Function struct (List of Basic Blocks).

- [X] **1.2 Build the CFG Builder**
  - Write a parser that reads a text file (e.g., input.ir) and constructs the Function object.
  - Requirement: It must handle labels and branches (JMP label, BEQ label).

- [X] **1.3 Visual Debugging (Critical)**
  - Implement dumpCFG(Function&) that outputs a generic .dot file.
  - Goal: Run dot -Tpng cfg.dot -o cfg.png and see your blocks connected correctly.

# Phase 2: Liveness Analysis (Weeks 3-4)

- [ ] **2.1 Local Liveness (Inside Blocks)**
  - Implement computeUseDef(): Calculate which variables are used/defined in each block before any other definition.

- [ ] **2.2 Global Liveness (The Solver)**
  - Implement the iterative solver for the Dataflow Equations:
    - $LiveOut[n] = \bigcup_{s \in succ[n]} LiveIn[s]$
    - $LiveIn[n] = Use[n] \cup (LiveOut[n] - Def[n])$
  - Constraint: Must handle loops (run until the sets stop changing).

- [ ] **2.3 Verify Liveness**
  - Add liveness sets to your Graphviz dump. Manually verify a simple loop.

# Phase 3: The Interference Graph (Week 5)

- [ ] **3.1 Build the Graph**
  - Create an adjacency matrix or list.
  - Iterate through all blocks. For every instruction, all variables currently in the "Live" set interfere with the variable being Defined. Add edges between them.

- [ ] **3.2 Visualize Conflict**
  - Implement dumpInterferenceGraph().
  - Goal: See a web of nodes.

# Phase 3.5: Move Coalescing (The Optimization)

- [ ] **3.3 Coalescing Analysis**
  - Identify all `MOV A, B` instructions.
  - Check: Do `A` and `B` interfere? (Are they connected in the graph?)
  - Optimization: If they do *not* interfere, **merge** them into a single node. This deletes the `MOV` instruction entirely from the final code.

- [ ] **3.4 Conservative Check (Briggs' Heuristic)**
  - Ensure merging doesn't make the graph uncolorable (i.e., the new node shouldn't have >= K neighbors of degree >= K).

# Phase 4: Allocation & Coloring (Week 6)

- [ ] **4.1 The Simplification Stack**
  - Implement the loop: Find node with Degree < K. Remove it. Push to stack.

- [ ] **4.2 The Optimistic Coloring**
  - Pop nodes. Assign the lowest available color ID (0 to K-1).

- [ ] **4.3 The "Register Map"**
  - If the stack empties successfully, output the final map: vreg1 -> R0, vreg2 -> R1.

# Phase 5: The "Widowmaker" (Spilling) (Week 7)

- [ ] **5.1 Loop Depth Analysis**
  - Write a pass that detects loops and marks blocks with their nesting depth (Depth 0, 1, 2...).

- [ ] **5.2 Spill Cost Heuristic**
  - Formula: Cost = (def_count + use_count) * (10 ^ loop_depth).
  - Select the node with the lowest cost to spill when the graph is uncolorable.

- [ ] **5.3 Rewrite the Code**
  - Insert STORE vReg, [StackOffset] after definitions.
  - Insert LOAD vReg, [StackOffset] before uses.
  - Crucial: Create new tiny virtual registers for the load/store temps to break the interference.

# Phase 6: Polish & Verification (Week 8)

- [ ] **6.1 The End-to-End Test**
  - Create a "Factorial" or "Fibonacci" input file (high register pressure).
  - Run iON.

- [ ] **6.2 Output Assembly**
  - Print the final code using physical registers (e.g., ADD R0, R1, R2).

- [ ] **6.3 Portfolio README**
  - Take screenshots of the "Before" (Spaghetti Graph) and "After" (Clean Colored Graph).

- [ ] **6.4 The "Stress Test" (Fuzzing)**
  - Write a Python script that generates random valid IR files (random ops, random variables).
  - Run iON on 1,000 generated files.
  - Goal: Zero crashes. This proves "Production Readiness."