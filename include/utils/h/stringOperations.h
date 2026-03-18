#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <format>
#include <optional>

constexpr std::string_view trim(std::string_view sv);
std::optional<std::string> extract_label_definition(std::string_view line);