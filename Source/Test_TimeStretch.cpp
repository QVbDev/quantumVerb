/*
  ==============================================================================

    Test_TimeStretch.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "TimeStretch.h"

#include <chrono>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Use a TimeStretch object to manipulate a buffer", "[TimeStretch]") {
    // Test config
    constexpr auto PROC_SAMPLE_RATE = 88200;    // Hz
    constexpr auto PROC_NUM_CHANNELS = 2;
    constexpr auto PROC_NUM_SAMPLES_PER_BLOCK = 0.02 * PROC_SAMPLE_RATE;

    // Prepare TimeStretch object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(PROC_NUM_CHANNELS, PROC_NUM_CHANNELS,
                                   PROC_SAMPLE_RATE, PROC_NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.getSampleRate() == PROC_SAMPLE_RATE);

    reverb::TimeStretch timeStretch(&processor);

    SECTION("Stretch audio buffer (44.1 kHz -> 88.2 kHz)") {
        constexpr auto IR_DURATION = 2.5;           // seconds
        constexpr auto IR_ORIG_SAMPLE_RATE = 44100; // Hz
        constexpr auto IR_ORIG_NUM_SAMPLES = IR_DURATION * IR_ORIG_SAMPLE_RATE;
        constexpr auto IR_EXPECTED_NUM_SAMPLES = IR_DURATION * PROC_SAMPLE_RATE;

        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(10);

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(PROC_NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == PROC_NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.origIRSampleRate = IR_ORIG_SAMPLE_RATE;

        REQUIRE(timeStretch.origIRSampleRate == IR_ORIG_SAMPLE_RATE);

        // Run TimeStretch, measuring time if optimisation is enabled
#ifdef MEASURE_PERFORMANCE
        auto start = std::chrono::high_resolution_clock::now();
#endif

        timeStretch.exec(ir);

#ifdef MEASURE_PERFORMANCE
        auto end = std::chrono::high_resolution_clock::now();
#endif

        REQUIRE(ir.getNumChannels() == PROC_NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);

#ifdef MEASURE_PERFORMANCE
        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
#endif
    }

    SECTION("Compress audio buffer (96 kHz -> 88.2 kHz)") {
        constexpr auto IR_DURATION = 5;             // seconds
        constexpr auto IR_ORIG_SAMPLE_RATE = 96000; // Hz
        constexpr auto IR_ORIG_NUM_SAMPLES = IR_DURATION * IR_ORIG_SAMPLE_RATE;
        constexpr auto IR_EXPECTED_NUM_SAMPLES = IR_DURATION * PROC_SAMPLE_RATE;

        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(10);

        // Prepare "audio" buffer
        juce::AudioSampleBuffer ir(PROC_NUM_CHANNELS, IR_ORIG_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == PROC_NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_ORIG_NUM_SAMPLES);

        // Prepare TimeStretch
        timeStretch.origIRSampleRate = IR_ORIG_SAMPLE_RATE;

        REQUIRE(timeStretch.origIRSampleRate == IR_ORIG_SAMPLE_RATE);

        // Run TimeStretch
#ifdef MEASURE_PERFORMANCE
        auto start = std::chrono::high_resolution_clock::now();
#endif

        timeStretch.exec(ir);

#ifdef MEASURE_PERFORMANCE
        auto end = std::chrono::high_resolution_clock::now();
#endif

        REQUIRE(ir.getNumChannels() == PROC_NUM_CHANNELS);
        REQUIRE(ir.getNumSamples() == IR_EXPECTED_NUM_SAMPLES);

#ifdef MEASURE_PERFORMANCE
        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
#endif
    }
}
