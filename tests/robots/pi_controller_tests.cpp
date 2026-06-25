#include "support/test_helpers.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "control/PIController.h"

TEST_CASE("PI controller uses max output without gains", "[Robot][PIController]") {
    PIController controller;

    CHECK(controller.update({10.0f, 1.0f, 25.0f}) == Catch::Approx(25.0f).margin(test::epsilon));
}

TEST_CASE("PI controller clamps output", "[Robot][PIController]") {
    PIController controller({2.0f, 1.0f, 100.0f});

    CHECK(controller.update({100.0f, 1.0f, 50.0f}) == Catch::Approx(50.0f).margin(test::epsilon));
}

TEST_CASE("PI controller reset clears integral", "[Robot][PIController]") {
    PIController controller({0.0f, 1.0f, 100.0f});

    CHECK(controller.update({10.0f, 1.0f, 100.0f}) == Catch::Approx(10.0f).margin(test::epsilon));
    CHECK(controller.update({10.0f, 1.0f, 100.0f}) == Catch::Approx(20.0f).margin(test::epsilon));

    controller.reset();

    CHECK(controller.update({10.0f, 1.0f, 100.0f}) == Catch::Approx(10.0f).margin(test::epsilon));
}
