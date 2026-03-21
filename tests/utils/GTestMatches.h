#pragma once

#include <gmock/gmock.h>
#include <variant>

MATCHER_P(IsOperandInt, expected_value, 
          "holds an integer with value " + ::testing::PrintToString(expected_value)) {
    
    if (!std::holds_alternative<int>(arg)) {
        *result_listener << "which currently holds a VReg"; 
        return false;
    }
    
    const int actual_value = std::get<int>(arg);
    if (actual_value != expected_value) {
        *result_listener << "which holds the integer " << actual_value;
        return false;
    }
    
    return true;
}

MATCHER_P(IsOperandVReg, expected_vreg, 
          "holds a VReg matching " + ::testing::PrintToString(expected_vreg)) {
    
    if (!std::holds_alternative<VReg>(arg)) {
        *result_listener << "which currently holds the integer " << std::get<int>(arg); 
        return false;
    }
    
    const VReg& actual_vreg = std::get<VReg>(arg);
    
    if (!(actual_vreg == expected_vreg)) {
        *result_listener << "which holds a different VReg: " 
                         << ::testing::PrintToString(actual_vreg);
        return false;
    }
    
    return true;
}