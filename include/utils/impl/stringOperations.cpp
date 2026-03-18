#include "h/stringOperations.h"

constexpr std::string_view trim(std::string_view sv) noexcept {
    const auto start = sv.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) return {};         // Empty or all whitespace
    const auto end = sv.find_last_not_of(" \t\r\n");
    return sv.substr(start, end - start + 1);
}

std::optional<std::string> extract_label_definition(std::string_view line) noexcept {
    std::string_view trimmed = trim(line);
    
    if (trimmed.empty()) return std::nullopt;

    size_t colon_pos = trimmed.find(':');
    
    // If there's a colon, and it's not the very first character
    if (colon_pos != std::string_view::npos && colon_pos > 0) {
        // Extract the label name before the colon
        std::string_view label_name = trimmed.substr(0, colon_pos);
        
        // Ensure the label name doesn't contain spaces (to avoid false positives)
        if (label_name.find_first_of(" \t") == std::string_view::npos) {
            return std::string(label_name);
        }
    }
    
    return std::nullopt;
}