# IR Examples
The iON IR program is a very standard generic-ish IR. Currently it is very very small and limited. Maybe I will expand it in the future. I did not design the IR, I had Claude design it.


## Registers
Virtual registers are written `%N` where N is a non-negative integer. There is no declared limit on N. They are write-once per program (SSA-like intent, though not enforced by the parser).


## Operands
Every operand is one of three things:

- `%N` — a virtual register
- `N` — an integer immediate (positive or negative)
- `name` — a label reference (used only in branch/jump instructions)


## Instruction Format
```
OPCODE %def, use1, use2, ...
```

The first register after the opcode is always the def (destination). Everything after the first comma is a use. For instructions with no def (`BEQ`, `RET`, `JMP`, `STORE`), all operands are uses.


## Opcodes
| Opcode | Def | Uses | Meaning |
|--------|-----|------|---------|
| `MOV %d, src` | `%d` | `src` | Copy src into %d. src is a register or immediate |
| `ADD %d, %a, %b` | `%d` | `%a, %b` | `%d = %a + %b`. Second use can be immediate |
| `SUB %d, %a, %b` | `%d` | `%a, %b` | `%d = %a - %b`. Second use can be immediate |
| `MUL %d, %a, %b` | `%d` | `%a, %b` | `%d = %a * %b`. Second use can be immediate |
| `LOAD %d, %addr` | `%d` | `%addr` | Load from memory address in `%addr` into `%d` |
| `STORE %src, %addr` | none | `%src, %addr` | Store `%src` into memory address `%addr` |
| `JMP label` | none | `label` | Unconditional jump to label |
| `BEQ %a, %b, label` | none | `%a, %b, label` | Jump to label if `%a == %b`, else fall through |
| `RET %a` | none | `%a` | Return `%a`. Terminates the block |


## Labels and blocks
A label is written on its own line followed by a colon:
```
loop:
```
A label marks the start of a new basic block. Every block should end with exactly one terminator — `JMP`, `BEQ`, or `RET`. Placing a branch in the middle of a block is legal syntax but produces a broken CFG since the constructor only inspects the last instruction.


## Whitespace and Structure
- Instructions are separated by newlines
- Operands are separated by commas
- Blank lines are skipped
- Comments are not supported (the lexer has no comment handling)
- The percent sign % is part of the register token, not a separator