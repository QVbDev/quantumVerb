/*
  ==============================================================================

    Test_Gain.cpp

  ==============================================================================
*/

#include "catch.hpp"
#include "PluginProcessor.h"
#include "Gain.h"
#include <chrono>

#define compareFloats(a1, a2) std::abs(a2 - a1) <= 0.01

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

class GainMocked : public reverb::Gain
{
public:
    using Gain::Gain;

    float getGainFactor() { return gainFactor; }
    void setGainFactor(float f) { gainFactor = f; }
};

TEST_CASE("Gain Class is tested", "[Gain]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds DURATION_MS(300);
    const int NUM_SAMPLES = (int)std::ceil((DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create Mixer object
    reverb::AudioProcessor processor;

    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES);

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);

    GainMocked gain(&processor);


    SECTION("Apply gain to audio buffer") {
        constexpr float GAIN = 2.0f;

        // Create audio block
        juce::AudioSampleBuffer audio(1, NUM_SAMPLES);

        REQUIRE(audio.getNumChannels() == NUM_CHANNELS);
        REQUIRE(audio.getNumSamples() == NUM_SAMPLES);

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            audio.setSample(0, i, 1);
        }

        // Run gain
        gain.setGainFactor(GAIN);

        REQUIRE(compareFloats(gain.getGainFactor(), GAIN));

        gain.exec(audio);

        CHECK(audio.getNumChannels() == NUM_CHANNELS);
        CHECK(audio.getNumSamples() == NUM_SAMPLES);

        // Since we started with audio(i) = 1 for all i < NUM_SAMPLES, we
        // should now have audio(i) = GAIN for all i < NUM_SAMPLES.
        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            CHECK(compareFloats(audio.getSample(0, i), GAIN));
        }
    }

    SECTION("Performance_Testing") {
        constexpr std::chrono::microseconds MAX_EXEC_TIME_US(2000);
        constexpr float GAIN = 2.0f;

        // Create audio block
        juce::AudioSampleBuffer audio(1, NUM_SAMPLES);

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            audio.setSample(0, i, 1);
        }

        // Run gain
        gain.setGainFactor(GAIN);

        // Measure exec time
        auto start = std::chrono::high_resolution_clock::now();
        gain.exec(audio);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        CHECK(execTime.count() < MAX_EXEC_TIME_US.count());
    }
}
