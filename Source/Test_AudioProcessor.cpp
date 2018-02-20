/*
  ==============================================================================

    Test_AudioProcessor.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"

#include <chrono>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Test whole-processor behaviours", "[AudioProcessor]") {
    constexpr int SAMPLE_RATE = 88200; // Hz
    constexpr int NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20); // ms
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

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

        // First exec (with IR pipeline)
        processor.processBlock(audio, emptyMidi);

        REQUIRE(!processor.irPipeline->mustExec);

        // Second exec (no IR pipeline)
        processor.processBlock(audio, emptyMidi);

        REQUIRE(!processor.irPipeline->mustExec);
    }


    SECTION("Regular processing should be real-time") {
        // TODO: What should our max exec time be? Currently minimum real-time value
        //       (i.e. actual block time)
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(BLOCK_DURATION_MS);

        // Prepare impulse response
        processor.processBlock(audio, emptyMidi);

        // Measure total processing time for one block
        auto start = std::chrono::high_resolution_clock::now();
        processor.processBlock(audio, emptyMidi);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
    }
}
