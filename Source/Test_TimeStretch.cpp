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

    double getOrigIRSampleRate() { return origIRSampleRate; }
    float getIRLengthS() { return irLengthS; }
    static constexpr double getMaxIRLengthS() { return MAX_IR_LENGTH_S; }
};

TEST_CASE("Use a TimeStretch object to manipulate a buffer", "[TimeStretch]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create TimeStretch object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(compareFloats(processor.getSampleRate(), SAMPLE_RATE));

    TimeStretchMocked timeStretch(&processor);


    SECTION("Stretch audio buffer (2s @ 96kHz -> 4s @ 88.2kHz)") {
        constexpr double IR_ORIG_SAMPLE_RATE = 96000;
        constexpr std::chrono::seconds IR_ORIG_DURATION_S(2);
        constexpr int64_t IR_ORIG_NUM_SAMPLES = IR_ORIG_DURATION_S.count() * IR_ORIG_SAMPLE_RATE;

        constexpr std::chrono::seconds IR_TARGET_DURATION_S(4);
        constexpr int64_t IR_EXPECTED_NUM_SAMPLES = IR_TARGET_DURATION_S.count() * SAMPLE_RATE;

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.setOrigIRSampleRate(IR_ORIG_SAMPLE_RATE);

        REQUIRE(compareFloats(timeStretch.getOrigIRSampleRate(), (double)IR_ORIG_SAMPLE_RATE));

        auto irLength = processor.parameters.getParameterAsValue(processor.PID_IR_LENGTH);
        irLength.setValue(IR_TARGET_DURATION_S.count());

        timeStretch.updateParams(processor.parameters, processor.PID_IR_LENGTH);

        REQUIRE(compareFloats(timeStretch.getIRLengthS(), IR_TARGET_DURATION_S.count()));

        // Run TimeStretch
        timeStretch.exec(ir);

        CHECK(ir.getNumChannels() == NUM_CHANNELS);
        CHECK(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }


    SECTION("Compress audio buffer (4s @ 96kHz -> 2s @ 88.2kHz)") {
        constexpr double IR_ORIG_SAMPLE_RATE = 96000;
        constexpr std::chrono::seconds IR_ORIG_DURATION_S(4);
        constexpr int64_t IR_ORIG_NUM_SAMPLES = IR_ORIG_DURATION_S.count() * IR_ORIG_SAMPLE_RATE;

        constexpr std::chrono::seconds IR_TARGET_DURATION_S(2);
        constexpr int64_t IR_EXPECTED_NUM_SAMPLES = IR_TARGET_DURATION_S.count() * SAMPLE_RATE;

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.setOrigIRSampleRate(IR_ORIG_SAMPLE_RATE);

        REQUIRE(timeStretch.getOrigIRSampleRate() == (double)IR_ORIG_SAMPLE_RATE);

        auto irLength = processor.parameters.getParameterAsValue(processor.PID_IR_LENGTH);
        irLength.setValue(IR_TARGET_DURATION_S.count());

        timeStretch.updateParams(processor.parameters, processor.PID_IR_LENGTH);

        REQUIRE(compareFloats(timeStretch.getIRLengthS(), IR_TARGET_DURATION_S.count()));

        // Run TimeStretch
        timeStretch.exec(ir);

        CHECK(ir.getNumChannels() == NUM_CHANNELS);
        CHECK(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }


    SECTION("IR should be limited to MAX_IR_LENGTH_S") {
        constexpr int IR_ORIG_SAMPLE_RATE = 44100;
        constexpr std::chrono::seconds IR_ORIG_DURATION_S(1);
        constexpr int64_t IR_ORIG_NUM_SAMPLES = IR_ORIG_DURATION_S.count() * IR_ORIG_SAMPLE_RATE;

        const double MAX_IR_LENGTH_S = timeStretch.getMaxIRLengthS();
        const std::chrono::seconds IR_TARGET_DURATION_S((int)std::ceil(MAX_IR_LENGTH_S + 1));
        const int64_t IR_EXPECTED_NUM_SAMPLES = IR_TARGET_DURATION_S.count() * SAMPLE_RATE;

        REQUIRE(IR_TARGET_DURATION_S.count() > MAX_IR_LENGTH_S);

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.setOrigIRSampleRate(IR_ORIG_SAMPLE_RATE);

        REQUIRE(compareFloats(timeStretch.getOrigIRSampleRate(), (double)IR_ORIG_SAMPLE_RATE));

        auto irLength = processor.parameters.getParameterAsValue(processor.PID_IR_LENGTH);

        timeStretch.exec(ir);

        CHECK(ir.getNumSamples() < MAX_IR_LENGTH_S * SAMPLE_RATE);
    }

}
