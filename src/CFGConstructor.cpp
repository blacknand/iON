#include "CFGConstructor.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// NOTE: helper, may want to move elsewhere (probably include/)
static std::string getTargetLabel(const Instruction& instr) {
    // Returns the right most label in the Instruction
    for (auto it = instr.uses.rbegin(); it != instr.uses.rend(); ++it) {
        if (std::holds_alternative<Label>(*it)) {
            return std::get<Label>(*it).name;
        }
    }
    throw std::runtime_error("Branch instruction missing Label operand.");
}

void CFGConstructor::construct(Function& fn) {
    // Map the labels to the blocks
    std::map<std::string, BasicBlock*> labelMap;
    for (const auto& blockPtr : fn.blocks) {
        if (!blockPtr->label.empty()) {
            labelMap[blockPtr->label] = blockPtr.get();
        }
    }

    // Wire the edges
    for (size_t i = 0; i < fn.blocks.size(); ++i) {
        BasicBlock* current = fn.blocks[i].get();

        // Handle empty blocks
        if (current->instrs.empty()) continue;

        const Instruction& last = current->instrs.back();

        // Unconditional jump
        if (last.op == Opcode::JMP) {
            std::string targetLabel = getTargetLabel(last);
            if (labelMap.find(targetLabel) == labelMap.end())
                throw std::runtime_error("Undefined label: " + targetLabel);
            BasicBlock* target = labelMap[targetLabel];
            addEdge(current, target);
        } 
        // Conditional branch
        else if (last.op == Opcode::BEQ) {
            std::string targetLabel = getTargetLabel(last);
            if (labelMap.find(targetLabel) == labelMap.end())
                throw std::runtime_error("Undefined label: " + targetLabel);

            BasicBlock* target = labelMap[targetLabel];
            addEdge(current, target);

            // Implicit fall through (next block in list if condition not true)
            if (i + 1 < fn.blocks.size())
                addEdge(current, fn.blocks[i+1].get());
        }
        // Return
        else if (last.op == Opcode::RET) {
            // No successors
        } 
        // Implicit fallthrough
        else {
            if (i + 1 < fn.blocks.size())
                addEdge(current, fn.blocks[i+1].get());
        }
    }
}

void CFGConstructor::addEdge(BasicBlock* from, BasicBlock* to) {
    if (!from || !to) return;

    auto& edges = from->succs;
    if (std::find(edges.begin(), edges.end(), to) != edges.end()) return;     // Already connected

    from->succs.push_back(to);
    to->preds.push_back(from);
}

std::string CFGConstructor::operandToString(const Operand& op) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, VReg>)
            return "%" + std::to_string(arg.id);
        else if constexpr (std::is_same_v<T, Imm>)
            return std::to_string(arg.value);
        else
            return arg.name;
    }, op);
}

std::string CFGConstructor::instrToString(const Instruction& instr) {
    std::ostringstream ss;

    // opcode
    switch (instr.op) {
        case Opcode::ADD:   ss << "ADD";   break;
        case Opcode::SUB:   ss << "SUB";   break;
        case Opcode::MUL:   ss << "MUL";   break;
        case Opcode::MOV:   ss << "MOV";   break;
        case Opcode::LOAD:  ss << "LOAD";  break;
        case Opcode::STORE: ss << "STORE"; break;
        case Opcode::JMP:   ss << "JMP";   break;
        case Opcode::BEQ:   ss << "BEQ";   break;
        case Opcode::RET:   ss << "RET";   break;
    }

    // def
    if (std::holds_alternative<VReg>(instr.def))
        ss << " %" << std::get<VReg>(instr.def).id << ",";

    // uses
    for (size_t i = 0; i < instr.uses.size(); ++i) {
        ss << " " << operandToString(instr.uses[i]);
        if (i + 1 < instr.uses.size()) ss << ",";
    }

    return ss.str();
}

std::string CFGConstructor::vregSetToString(const std::set<int>& s) {
    if (s.empty()) return "∅";
    std::ostringstream ss;
    ss << "{";
    bool first = true;
    for (int id : s) {
        if (!first) ss << ", ";
        ss << "%" << id;
        first = false;
    }
    ss << "}";
    return ss.str();
}

// Build an HTML-like DOT label for a single basic block.
// Optionally annotates LiveIn (top) and LiveOut (bottom).
std::string CFGConstructor::blockLabel(const BasicBlock& bb,
                                       const std::set<int>* liveIn,
                                       const std::set<int>* liveOut) {
    std::ostringstream ss;

    // DOT HTML table
    ss << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">";

    // ── Header row: block id + label ──────────────────────────
    std::string header = "BB" + std::to_string(bb.id);
    if (!bb.label.empty()) header += " (" + bb.label + ")";
    ss << "<TR><TD BGCOLOR=\"#4A90D9\"><FONT COLOR=\"white\"><B>"
       << header << "</B></FONT></TD></TR>";

    // ── LiveIn row ────────────────────────────────────────────
    if (liveIn) {
        ss << "<TR><TD BGCOLOR=\"#D4EDDA\"><FONT POINT-SIZE=\"9\">"
           << "<B>LiveIn:</B> " << vregSetToString(*liveIn)
           << "</FONT></TD></TR>";
    }

    // ── Instructions ──────────────────────────────────────────
    for (const auto& instr : bb.instrs) {
        ss << "<TR><TD ALIGN=\"LEFT\"><FONT FACE=\"Courier\">"
           << instrToString(instr)
           << "</FONT></TD></TR>";
    }

    // ── LiveOut row ───────────────────────────────────────────
    if (liveOut) {
        ss << "<TR><TD BGCOLOR=\"#FFF3CD\"><FONT POINT-SIZE=\"9\">"
           << "<B>LiveOut:</B> " << vregSetToString(*liveOut)
           << "</FONT></TD></TR>";
    }

    ss << "</TABLE>";
    return ss.str();
}

// Phase 1.3: dumpCFG — no liveness
void CFGConstructor::dumpCFG(const Function& fn, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    out << "digraph " << fn.name << " {\n";
    out << "  graph [rankdir=TB, fontname=\"Helvetica\"];\n";
    out << "  node  [shape=plaintext];\n\n";

    // Nodes
    for (const auto& bp : fn.blocks) {
        out << "  BB" << bp->id
            << " [label=<" << blockLabel(*bp) << ">];\n";
    }

    out << "\n";

    // Edges
    for (const auto& bp : fn.blocks) {
        for (BasicBlock* succ : bp->succs) {
            out << "  BB" << bp->id << " -> BB" << succ->id << ";\n";
        }
    }

    out << "}\n";
    out.flush();
}

// Phase 2.3: dumpCFGWithLiveness — annotates LiveIn / LiveOut
void CFGConstructor::dumpCFGWithLiveness(
    const Function& fn,
    const std::map<int, std::set<int>>& liveIn,
    const std::map<int, std::set<int>>& liveOut,
    const std::string& filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    out << "digraph " << fn.name << "_liveness {\n";
    out << "  graph [rankdir=TB, fontname=\"Helvetica\", label=\"Liveness Analysis: "
        << fn.name << "\", labelloc=t];\n";
    out << "  node  [shape=plaintext];\n\n";

    // Nodes
    for (const auto& bp : fn.blocks) {
        int id = bp->id;

        const std::set<int>* in  = liveIn.count(id)  ? &liveIn.at(id)  : nullptr;
        const std::set<int>* out_ = liveOut.count(id) ? &liveOut.at(id) : nullptr;

        out << "  BB" << id
            << " [label=<" << blockLabel(*bp, in, out_) << ">];\n";
    }

    out << "\n";

    // Edges
    for (const auto& bp : fn.blocks) {
        for (BasicBlock* succ : bp->succs) {
            out << "  BB" << bp->id << " -> BB" << succ->id
                << " [penwidth=1.5];\n";
        }
    }

    out << "}\n";
    out.flush();
}
