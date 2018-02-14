/*
  ==============================================================================

    Test_IRPipeline.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "IRPipeline.h"
#include "PluginProcessor.h"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Use an IRPipeline to manipulate an impulse response", "[IRPipeline]") {
    constexpr int IR_SAMPLE_RATE = 88200;
    constexpr int IR_NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds IR_DURATION_MS(5000);
    constexpr double IR_NUM_SAMPLES = (IR_DURATION_MS.count() / 1000.0) * IR_SAMPLE_RATE;

    // Create IRPipeline object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(IR_NUM_CHANNELS, IR_NUM_CHANNELS,
                                   IR_SAMPLE_RATE, std::ceil(IR_NUM_SAMPLES));

    REQUIRE(processor.getSampleRate() == IR_SAMPLE_RATE);

    reverb::IRPipeline irPipeline(&processor);


    SECTION("IR processing shouldn't be excessively long") {
        constexpr std::chrono::seconds MAX_EXEC_TIME_MS(200);

        juce::AudioSampleBuffer ir;

        // Measure processing time
        auto start = std::chrono::high_resolution_clock::now();
        irPipeline.exec(ir);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
    }
}
