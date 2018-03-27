/*
  ==============================================================================

    Test_TimeStretch.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "TimeStretch.h"

#include <algorithm>
#include <chrono>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

#define compareFloats(a1, a2) std::abs(a2 - a1) <= 0.01

class TimeStretchMocked : public reverb::TimeStretch
{
public:
    using TimeStretch::TimeStretch;

    float getIRLengthS() { return irLengthS; }
    static constexpr double getMaxIRLengthS() { return MAX_IR_LENGTH_S; }
};

TEST_CASE("Use a TimeStretch object to manipulate a buffer", "[TimeStretch]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create TimeStretch object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(compareFloats(processor.getSampleRate(), SAMPLE_RATE));

    TimeStretchMocked timeStretch(&processor);
    timeStretch.updateSampleRate(SAMPLE_RATE);


    SECTION("Stretch audio buffer (2s @ 96kHz -> 4s @ 88.2kHz)") {
        constexpr double IR_ORIG_SAMPLE_RATE = 96000;
        constexpr std::chrono::seconds IR_ORIG_DURATION_S(2);
        const int IR_ORIG_NUM_SAMPLES = (int)std::ceil(IR_ORIG_DURATION_S.count() * IR_ORIG_SAMPLE_RATE);

        constexpr std::chrono::seconds IR_TARGET_DURATION_S(4);
        const int IR_EXPECTED_NUM_SAMPLES = (int)std::ceil(IR_TARGET_DURATION_S.count() * SAMPLE_RATE);

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        auto irLength = processor.parameters.getParameterAsValue(processor.PID_IR_LENGTH);
        irLength.setValue(IR_TARGET_DURATION_S.count());

        timeStretch.updateParams(processor.parameters, processor.PID_IR_LENGTH);

        REQUIRE(compareFloats(timeStretch.getIRLengthS(), IR_TARGET_DURATION_S.count()));

        // Run TimeStretch
        timeStretch.prepareIR(ir);
        timeStretch.exec(ir);

        CHECK(ir.getNumChannels() == NUM_CHANNELS);
        CHECK(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }


    SECTION("Compress audio buffer (4s @ 96kHz -> 2s @ 88.2kHz)") {
        constexpr double IR_ORIG_SAMPLE_RATE = 96000;
        constexpr std::chrono::seconds IR_ORIG_DURATION_S(4);
        const int IR_ORIG_NUM_SAMPLES = (int)std::ceil(IR_ORIG_DURATION_S.count() * IR_ORIG_SAMPLE_RATE);

        constexpr std::chrono::seconds IR_TARGET_DURATION_S(2);
        const int IR_EXPECTED_NUM_SAMPLES = (int)std::ceil(IR_TARGET_DURATION_S.count() * SAMPLE_RATE);

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        auto irLength = processor.parameters.getParameterAsValue(processor.PID_IR_LENGTH);
        irLength.setValue(IR_TARGET_DURATION_S.count());

        timeStretch.updateParams(processor.parameters, processor.PID_IR_LENGTH);

        REQUIRE(compareFloats(timeStretch.getIRLengthS(), IR_TARGET_DURATION_S.count()));

        // Run TimeStretch
        timeStretch.prepareIR(ir);
        timeStretch.exec(ir);

        CHECK(ir.getNumChannels() == NUM_CHANNELS);
        CHECK(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }

}
