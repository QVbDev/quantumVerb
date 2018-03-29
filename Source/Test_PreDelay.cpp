/*
  ==============================================================================

    Test_PreDelay.cpp

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

 //==============================================================================
 /**
 * Mocked PreDelay class to facilitate accessing protected members in unit tests.
 */
class PreDelayMocked : public reverb::PreDelay
{
public:
    using PreDelay::PreDelay;

    void setDelayMs(float delay) { delayMs = delay; }
    float getDelayMs() { return delayMs; }

    constexpr int getMaxDelayMs() { return MAX_DELAY_MS; }
};

TEST_CASE("Use a PreDelay object to manipulate an impulse response", "[PreDelay]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create PreDelay object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);

    PreDelayMocked preDelay(&processor);
    preDelay.updateSampleRate(SAMPLE_RATE);

    // Prepare dummy IR
    constexpr int IR_SIZE = 2048;
    constexpr float IR_VAL_OFFSET = 1000000.0f;

    juce::AudioSampleBuffer ir(1, IR_SIZE);

    for (int i = 0; i < IR_SIZE; ++i)
    {
        ir.setSample(0, i, IR_VAL_OFFSET + i);
    }


    SECTION("When delay is 0, IR buffer should remain unchanged") {
        preDelay.setDelayMs(0);

        REQUIRE(preDelay.getDelayMs() == 0);

        preDelay.prepareIR(ir);
        preDelay.exec(ir);

        CHECK(ir.getNumChannels() == 1);
        CHECK(ir.getNumSamples() == IR_SIZE);

        for (int i = 0; i < IR_SIZE; ++i)
        {
            CHECK(ir.getSample(0, i) == IR_VAL_OFFSET + i);
        }
    }


    SECTION("Add 1ms pre-delay to IR => prepend 89 empty samples to buffer") {
        static constexpr double DELAY_S = 0.001;
        const int EXPECTED_NUM_SAMPLES = (int)std::ceil(SAMPLE_RATE * DELAY_S);

        preDelay.setDelayMs(DELAY_S * 1000);

        REQUIRE(preDelay.getDelayMs() == DELAY_S * 1000);

        preDelay.prepareIR(ir);
        preDelay.exec(ir);

        CHECK(ir.getNumChannels() == 1);
        CHECK(ir.getNumSamples() == IR_SIZE + EXPECTED_NUM_SAMPLES);

        for (int i = 0; i < EXPECTED_NUM_SAMPLES; ++i)
        {
            CHECK(ir.getSample(0, i) == 0);
        }

        for (int i = EXPECTED_NUM_SAMPLES; i < ir.getNumSamples(); ++i)
        {
            CHECK(ir.getSample(0, i) == IR_VAL_OFFSET + (i - EXPECTED_NUM_SAMPLES));
        }
    }
  

    SECTION("Performance_Testing") {
        constexpr std::chrono::microseconds MAX_EXEC_TIME_US(1000);
        static constexpr double DELAY_S = 1;
        const int EXPECTED_NUM_SAMPLES = (int)std::ceil(SAMPLE_RATE * DELAY_S);

        preDelay.setDelayMs(DELAY_S * 1000);

        // Measure exec time
        auto start = std::chrono::high_resolution_clock::now();
        preDelay.prepareIR(ir);
        preDelay.exec(ir);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        CHECK(execTime.count() < MAX_EXEC_TIME_US.count());
    }
    
}
