/*
  ==============================================================================

    Test_AudioProcessor.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Test whole-processor behaviours", "[AudioProcessor]") {
    // Test config
    constexpr auto PROC_SAMPLE_RATE = 88200;    // Hz
    constexpr auto PROC_NUM_CHANNELS = 2;
    constexpr auto PROC_NUM_SAMPLES_PER_BLOCK = 0.02 * PROC_SAMPLE_RATE;

    // Create AudioProcessor
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(PROC_NUM_CHANNELS, PROC_NUM_CHANNELS,
                                   PROC_SAMPLE_RATE, PROC_NUM_SAMPLES_PER_BLOCK);

    // TODO: REQUIRE(checks)
    // TODO: SECTION("Ensure IRPipeline is only run when necessary")
}
