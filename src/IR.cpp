#include "IR.h"

// Pretty prints CFG
void Instruction::dump() const {
    std::cout << "Opcode: " << op << ", ";

    // Output destination
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, VReg>) {
            std::cout << "%" << arg.id << ", ";
        } else if constexpr (std::is_same_v<T, std::monostate>) {
            // ...
        }
    }, def);

    // Output sources
    for (const auto& op : uses) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, VReg>) {
                std::cout << "%" << arg.id << ", ";
            } else if constexpr (std::is_same_v<T, Imm>) {
                std::cout << arg.value << ", ";
            } else if constexpr (std::is_same_v<T, Label>) {
                std::cout << arg.name << ", ";
            }
            std::cout << " ";
        }, op);
    }

    std::cout << "\n";
}