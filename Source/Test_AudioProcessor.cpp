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

 //==============================================================================
 /**
 * Mocked AudioProcessor class to facilitate accessing protected members in unit tests.
 */
class AudioProcessorMocked : public reverb::AudioProcessor
{
public:
    using AudioProcessor::AudioProcessor;

    reverb::IRPipeline::Ptr getIRPipeline(int channelIdx) { return irPipelines[channelIdx]; }
    reverb::MainPipeline::Ptr getMainPipeline(int channelIdx) { return mainPipelines[channelIdx]; }

    juce::AudioSampleBuffer& getIRChannel(int channelIdx) { return irChannels[channelIdx]; }
    juce::AudioSampleBuffer& getAudioChannel(int channelIdx) { return audioChannels[channelIdx]; }
};

TEST_CASE("Test whole-processor behaviours", "[AudioProcessor]") {
    constexpr int SAMPLE_RATE = 88200; // Hz
    constexpr int NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20); // ms
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create AudioProcessor
    AudioProcessorMocked processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    processor.prepareToPlay(SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        REQUIRE(processor.getIRPipeline(i) != nullptr);
        REQUIRE(processor.getMainPipeline(i) != nullptr);
    }

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

    // Create dummy midi buffer for processBlock() method
    juce::MidiBuffer midi;


    SECTION("IRPipeline should be disabled after execution") {
        // First exec (with IR pipeline)
        for (int i = 0; i < NUM_CHANNELS; ++i)
        {
            REQUIRE(processor.getIRPipeline(i)->needsToRun());
        }

        auto start = std::chrono::high_resolution_clock::now();
        processor.processBlock(audio, midi);
        auto end = std::chrono::high_resolution_clock::now();

        auto firstExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        for (int i = 0; i < NUM_CHANNELS; ++i)
        {
            CHECK(!processor.getIRPipeline(i)->needsToRun());
        }

        // Second exec (no IR pipeline)
        start = std::chrono::high_resolution_clock::now();
        processor.processBlock(audio, midi);
        end = std::chrono::high_resolution_clock::now();

        auto secondExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        for (int i = 0; i < NUM_CHANNELS; ++i)
        {
            CHECK(!processor.getIRPipeline(i)->needsToRun());
        }

        CHECK(secondExecTime.count() < firstExecTime.count() - 10);
    }


    SECTION("Regular processing should be real-time") {
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(BLOCK_DURATION_MS);
        constexpr int NUM_ITERATIONS = 100;

        // Prepare impulse response
        processor.prepareToPlay(SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);
        processor.processBlock(audio, midi);

        for (int i = 0; i < NUM_ITERATIONS; ++i)
        {
            // Measure total processing time for one block
            auto start = std::chrono::high_resolution_clock::now();

            processor.prepareToPlay(SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);
            processor.processBlock(audio, midi);

            auto end = std::chrono::high_resolution_clock::now();

            auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
#ifdef NDEBUG
            CHECK(execTime.count() < MAX_EXEC_TIME_MS.count());
#else
            CHECK(execTime.count() < 2 * MAX_EXEC_TIME_MS.count());
#endif
        }
    }
}
