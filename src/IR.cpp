#include "IR.h"

// Pretty prints CFG
void Instruction::dump() const {
    std::cout << "Opcode: " << op << ", ";

    // Output destination
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same<T, VReg>) {
            std::cout << "%" << T.id << ", ";
        } else if (std::holds_alternative<std::monostate>(T)) {
            ...
        }
    }, def);

    // Output sources
    for (const auto& op : uses) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same<T, VReg>) {
                std::cout << "%" << T.id << ", ";
            } else if constexpr (std::is_same<T, Imm>) {
                std::cout << T.value << ", ";
            } else if constexpr (std::is_same<T, Label>) {
                std::cout << T.label << ", ";
            }
            std::cout << " ";
        }, op)
    }

    std::cout << "\n";
}