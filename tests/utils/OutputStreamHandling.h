#pragma once

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

class OutputCapture : public ::testing::EmptyTestEventListener {
    std::ostringstream captured_out_;
    std::ostringstream captured_err_;
    std::streambuf* old_cout_ = nullptr;
    std::streambuf* old_cerr_ = nullptr;

public:
    void OnTestProgramStart(const ::testing::UnitTest&) override {
        old_cout_ = std::cout.rdbuf(captured_out_.rdbuf());
        old_cerr_ = std::cerr.rdbuf(captured_err_.rdbuf());
    }

    void OnTestProgramEnd(const ::testing::UnitTest&) override {
        // Restore streams first
        std::cout.rdbuf(old_cout_);
        std::cerr.rdbuf(old_cerr_);

        // Now dump everything
        std::string out = captured_out_.str();
        std::string err = captured_err_.str();

        if (!out.empty()) {
            std::cout << "\n===== Captured stdout =====\n"
                      << out
                      << "===========================\n";
        }
        if (!err.empty()) {
            std::cerr << "\n===== Captured stderr =====\n"
                      << err
                      << "===========================\n";
        }
    }
};