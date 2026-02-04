#include <catch2/catch_test_macros.hpp>
#include <ion/IR.H>

TEST_CASE("IR basic functionality", "[IR]") {
    SECTION("basic test") {
        REQUIRE(1 + 1 == 2);
    }

    SECTION("another test") {
        REQUIRE(true);
    }
}