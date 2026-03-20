#include "utils/h/Parser.h"

std::optional<ParsedInstr> InstrParser::parse(std::string_view line) {
    line = trim(line);
    if (line.empty()) return std::nullopt;

    // --- Label definition ---
    if (line.back() == ':') {
        return ParsedInstr{
            .form = ParsedInstr::LabelDef,
            .label = line.substr(0, line.size() - 1)
        };
    }

    // --- Tokenize ---
    std::array<std::string_view, 6> toks;
    int n = tokenize(line, toks);
    if (n == 0) return std::nullopt;

    ParsedInstr instr{};
    instr.opcode = toks[0];
    Form form = classify(toks[0]);
    instr.form = form;

    // Converts a parsed Operand token into the IR Operands variant
    auto toOperands = [](Operand op) -> Operands {
        return (op.kind == Operand::VR) ? Operands{VReg{op.value}} : Operands{op.value};
    };

    switch (form) {
    case ParsedInstr::Ret:
        // RET — nothing to extract
        break;

    case ParsedInstr::Jump:
        // JMP TARGET
        instr.targets[0] = toks[1];
        instr.target_count = 1;
        break;

    case ParsedInstr::LoadImm:
        // LOADI %1, 42
        instr.def = VReg{parse_operand(toks[1]).value};
        instr.uses[0] = toOperands(parse_operand(toks[2]));
        instr.use_count = 1;
        break;

    case ParsedInstr::Copy:
        // MOV %1, %2  (def = %1, use = %2)
        instr.def = VReg{parse_operand(toks[1]).value};
        instr.uses[0] = toOperands(parse_operand(toks[2]));
        instr.use_count = 1;
        break;

    case ParsedInstr::BinaryOp:
        // ADD %1, %2, %3  (def = %1, uses = %2, %3)
        instr.def = VReg{parse_operand(toks[1]).value};
        instr.uses[0] = toOperands(parse_operand(toks[2]));
        instr.uses[1] = toOperands(parse_operand(toks[3]));
        instr.use_count = 2;
        break;

    case ParsedInstr::CondBranch1:
        // NZ %1, T1, T2
        instr.uses[0] = toOperands(parse_operand(toks[1]));
        instr.use_count = 1;
        instr.targets[0] = toks[2];
        instr.targets[1] = toks[3];
        instr.target_count = 2;
        break;

    case ParsedInstr::CondBranch2:
        // BEQ %1, %2, T1, T2
        instr.uses[0] = toOperands(parse_operand(toks[1]));
        instr.uses[1] = toOperands(parse_operand(toks[2]));
        instr.use_count = 2;
        instr.targets[0] = toks[3];
        instr.targets[1] = toks[4];
        instr.target_count = 2;
        break;

    default:
        return std::nullopt;
    }

    return instr;
}


// Map opcode string -> form.
InstrParser::Form InstrParser::classify(std::string_view op) {
    // Binary ops
    if (op == "ADD" || op == "SUB" || op == "MUL" ||
        op == "DIV" || op == "AND" || op == "OR")
        return Form::BinaryOp;

    // Branches
    if (op == "BEQ" || op == "BNE" || op == "BLT" || op == "BGT")
        return Form::CondBranch2;
    if (op == "NZ" || op == "BZ")
        return Form::CondBranch1;

    if (op == "MOV")    return Form::Copy;
    if (op == "LOADI")  return Form::LoadImm;
    if (op == "JMP")    return Form::Jump;
    if (op == "RET")    return Form::Ret;

    return Form::Ret; // fallback — you'd want an error path here
}

// Split line on commas and whitespace into token buffer. Zero alloc.
int InstrParser::tokenize(std::string_view line, std::array<std::string_view, 6>& out) {
    int count = 0;
    size_t i = 0;

    while (i < line.size() && count < static_cast<int>(out.size())) {
        // skip delimiters
        while (i < line.size() && (line[i] == ' ' || line[i] == ',' || line[i] == '\t'))
            ++i;
        if (i >= line.size()) break;

        // scan token
        size_t start = i;
        while (i < line.size() && line[i] != ' ' && line[i] != ',' && line[i] != '\t')
            ++i;

        out[count++] = line.substr(start, i - start);
    }
    return count;
}

Operand InstrParser::parse_operand(std::string_view tok) {
    if (tok.front() == '%') {
        int32_t reg = 0;
        std::from_chars(tok.data() + 1, tok.data() + tok.size(), reg);
        return Operand::vr(reg);
    }
    // constant (possibly negative)
    int32_t val = 0;
    std::from_chars(tok.data(), tok.data() + tok.size(), val);
    return Operand::imm(val);
}

std::string_view InstrParser::trim(std::string_view s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t'))  s.remove_suffix(1);
    return s;
}
