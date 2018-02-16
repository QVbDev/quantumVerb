/*
  ==============================================================================

    Test_PreDelay.cpp
    Created: 14 Feb 2018 3:27:04pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "PreDelay.h"

#include <chrono>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Use a PreDelay object to manipulate an impulse response", "[PreDelay]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    constexpr double NUM_SAMPLES_PER_BLOCK = (BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE;

    // Create PreDelay object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, std::ceil(NUM_SAMPLES_PER_BLOCK));

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);

    reverb::PreDelay preDelay(&processor);

    // Prepare dummy IR
    constexpr int IR_SIZE = 2048;
    constexpr double IR_VAL_OFFSET = 1000000.0;

    juce::AudioSampleBuffer ir(1, IR_SIZE);

    for (int i = 0; i < IR_SIZE; ++i)
    {
        ir.setSample(0, i, IR_VAL_OFFSET + i);
    }


    SECTION("When delay is 0, IR buffer should remain unchanged") {
        preDelay.delayMs = 0;

        REQUIRE(preDelay.delayMs == 0);

        preDelay.exec(ir);

        REQUIRE(ir.getNumChannels() == 1);
        REQUIRE(ir.getNumSamples() == IR_SIZE);

        for (int i = 0; i < IR_SIZE; ++i)
        {
            REQUIRE(ir.getSample(0, i) == IR_VAL_OFFSET + i);
        }
    }


    SECTION("Add 1ms pre-delay to IR => prepend 89 empty samples to buffer") {
        static constexpr double DELAY_S = 0.001;
        const double EXPECTED_NUM_SAMPLES = std::ceil(SAMPLE_RATE * DELAY_S);

        preDelay.delayMs = DELAY_S * 1000;

        REQUIRE(preDelay.delayMs == DELAY_S * 1000);

        preDelay.exec(ir);

        REQUIRE(ir.getNumChannels() == 1);
        REQUIRE(ir.getNumSamples() == IR_SIZE + EXPECTED_NUM_SAMPLES);

        for (int i = 0; i < EXPECTED_NUM_SAMPLES; ++i)
        {
            REQUIRE(ir.getSample(0, i) == 0);
        }

        for (int i = EXPECTED_NUM_SAMPLES; i < ir.getNumSamples(); ++i)
        {
            REQUIRE(ir.getSample(0, i) == IR_VAL_OFFSET + (i - EXPECTED_NUM_SAMPLES));
        }
    }


    SECTION("When pre-delay exceeds 1s, block should throw an exception") {
        preDelay.delayMs = preDelay.MAX_DELAY_MS + 1;

        REQUIRE(preDelay.delayMs == preDelay.MAX_DELAY_MS + 1);

        bool gotException = false;

        try
        {
            preDelay.exec(ir);
        }
        catch (std::invalid_argument)
        {
            gotException = true;
        }

        REQUIRE(gotException);
    }
}
