/*
  ==============================================================================

    Test_AudioProcessor.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"

#include <chrono>

#define MEASURE_PERFORMANCE 1

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Test whole-processor behaviours", "[AudioProcessor]") {
    // Test config
    constexpr auto SAMPLE_RATE = 88200;     // Hz
    constexpr auto NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);   // seconds
    constexpr auto NUM_SAMPLES_PER_BLOCK = BLOCK_DURATION_MS.count() * SAMPLE_RATE;

    // Create AudioProcessor
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.irPipeline != nullptr);
    REQUIRE(processor.mainPipeline != nullptr);

    // Create "audio" buffer and fill it with random data
    juce::AudioSampleBuffer audio(NUM_CHANNELS, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(audio.getNumChannels() == NUM_CHANNELS);
    REQUIRE(audio.getNumSamples() == NUM_SAMPLES_PER_BLOCK);

    juce::Random random;
    random.setSeedRandomly();

    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
    {
        auto writePtr = audio.getWritePointer(channel);

        for (size_t i = 0; i < audio.getNumSamples(); ++i)
        {
            writePtr[i] = random.nextFloat();
        }
    }

    // Create empty midi buffer
    juce::MidiBuffer emptyMidi;

    REQUIRE(emptyMidi.isEmpty());

    SECTION("IRPipeline should be disabled after execution") {
        REQUIRE(processor.irPipeline->mustExec);

        processor.processBlock(audio, emptyMidi);

        REQUIRE(!processor.irPipeline->mustExec);

        processor.processBlock(audio, emptyMidi);

        REQUIRE(!processor.irPipeline->mustExec);
    }

#ifdef MEASURE_PERFORMANCE
    SECTION("Processing should be real-time (less than audio block time)") {
        // Disable IRPipeline
        processor.processBlock(audio, emptyMidi);

        // Measure processing time
        auto start = std::chrono::high_resolution_clock::now();
        processor.processBlock(audio, emptyMidi);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        REQUIRE(execTime.count() < BLOCK_DURATION_MS.count());
    }
#endif
}
