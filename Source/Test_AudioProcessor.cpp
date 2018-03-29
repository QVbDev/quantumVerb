/*
  ==============================================================================

    Test_AudioProcessor.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"

#include <chrono>
#include <cmath>
#include <iostream>

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


    SECTION("Regular processing should be real-time") {
#ifdef NDEBUG
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(BLOCK_DURATION_MS);
        constexpr int NUM_ITERATIONS = 2000;
#else
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(5*BLOCK_DURATION_MS);
        constexpr int NUM_ITERATIONS = 100;
#endif

        // Prepare impulse response
        processor.processBlock(audio, midi);

        std::array<std::chrono::milliseconds, NUM_ITERATIONS> execTimes;

        for (int i = 0; i < NUM_ITERATIONS; ++i)
        {
            // Measure total processing time for one block
            auto start = std::chrono::high_resolution_clock::now();
            processor.processBlock(audio, midi);
            auto end = std::chrono::high_resolution_clock::now();

            execTimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            CHECK(execTimes[i].count() < MAX_EXEC_TIME_MS.count());
        }

        // Calculate some basic stats
        double execTimeAvg = 0;
        double execTimeDev = 0;

        for (auto& execTime : execTimes)
        {
            execTimeAvg += execTime.count();
        }

        execTimeAvg /= execTimes.size();

        std::cout << std::endl;
        std::cout << "Exec time average: " << execTimeAvg << " ms" << std::endl;

        for (auto& execTime : execTimes)
        {
            execTimeDev += execTime.count() - execTimeAvg;
            execTimeDev *= execTimeDev;
            execTimeDev /= execTimes.size() - 1;
        }

        execTimeDev = sqrt(execTimeDev);

        std::cout << "Exec time standard deviation: " << execTimeDev << " ms" << std::endl;
        std::cout << std::endl;

        REQUIRE(execTimeAvg < MAX_EXEC_TIME_MS.count());
    }
}
