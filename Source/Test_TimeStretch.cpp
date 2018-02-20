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

TEST_CASE("Use a TimeStretch object to manipulate a buffer", "[TimeStretch]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create TimeStretch object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);

    reverb::TimeStretch timeStretch(&processor);


    SECTION("Stretch audio buffer (44.1 kHz -> 88.2 kHz)") {
        constexpr int IR_ORIG_SAMPLE_RATE = 44100;
        constexpr std::chrono::seconds IR_DURATION_S(2);
        constexpr int64_t IR_ORIG_NUM_SAMPLES = IR_DURATION_S.count() * IR_ORIG_SAMPLE_RATE;
        constexpr int64_t IR_EXPECTED_NUM_SAMPLES = IR_DURATION_S.count() * SAMPLE_RATE;

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.origIRSampleRate = IR_ORIG_SAMPLE_RATE;

        REQUIRE(timeStretch.origIRSampleRate == IR_ORIG_SAMPLE_RATE);

        // Run TimeStretch
        timeStretch.exec(ir);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }


    SECTION("Compress audio buffer (96 kHz -> 88.2 kHz)") {
        constexpr int IR_ORIG_SAMPLE_RATE = 96000;
        constexpr std::chrono::seconds IR_DURATION_S(5);
        constexpr int64_t IR_ORIG_NUM_SAMPLES = IR_DURATION_S.count() * IR_ORIG_SAMPLE_RATE;
        constexpr int64_t IR_EXPECTED_NUM_SAMPLES = IR_DURATION_S.count() * SAMPLE_RATE;

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.origIRSampleRate = IR_ORIG_SAMPLE_RATE;

        REQUIRE(timeStretch.origIRSampleRate == (double)IR_ORIG_SAMPLE_RATE);

        // Run TimeStretch
        timeStretch.exec(ir);

        REQUIRE(ir.getNumChannels() == NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);
    }
}
