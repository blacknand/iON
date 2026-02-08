cmake_minimum_required(VERSION 3.20)
project(ion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ION_BUILD_TESTS "Build tests" ON)
option(ION_USE_GTEST "Use GoogleTest" ON)

# Main library (so tests can link against it)
add_library(ion_lib
    src/IR.cpp
    src/CFGConstructor.cpp
    src/IRParser.cpp
    src/Lexer.cpp
)
target_include_directories(ion_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/ion
)

# Compiler warnings for library
if(MSVC)
    target_compile_options(ion_lib PRIVATE /W4)
else()
    target_compile_options(ion_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable Sanitizers for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # ASan, LSan, UBSan
    add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

# Main executable
add_executable(ion src/main.cpp)
target_link_libraries(ion PRIVATE ion_lib)

# Testing
if(ION_BUILD_TESTS)
    enable_testing()
    include(FetchContent)

    # GoogleTest
    if(ION_USE_GTEST)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        # Prevent overriding parent project's compiler/linker settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)

        # GoogleTest executable
        add_executable(ion_test_gtest
            tests/test_main_gtest.cpp
            tests/test_ir.cpp
            # Add more test files here
        )
        target_link_libraries(ion_test_gtest PRIVATE ion_lib GTest::gtest_main)
        
        include(GoogleTest)
        gtest_discover_tests(ion_test_gtest)
    endif()
endif()