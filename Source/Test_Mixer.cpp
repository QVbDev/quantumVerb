/*
  ==============================================================================

    Test_Mixer.cpp

  ==============================================================================
*/

#include "catch.hpp"
#include "PluginProcessor.h"
#include "Mixer.h"
#include <chrono>


/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

#define compareFloats(a1, a2) std::abs(a2 - a1) <= 0.01

class MixerMocked : public reverb::Mixer
{
public:
    using Mixer::Mixer;

    float getWetRatio() { return wetRatio; }
    void setWetRatio(float r) { wetRatio = r; }
};

TEST_CASE("Mixer class is tested", "[Mixer]")
{
	constexpr int SAMPLE_RATE = 88200;
	constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds DURATION_MS(300);
    const int NUM_SAMPLES = (int)std::ceil((DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

	// Create Mixer object
	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
		                           SAMPLE_RATE, NUM_SAMPLES);

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);
    MixerMocked mixer(&processor);

    SECTION("Mix two audio buffers") {
        constexpr float WET_RATIO = 0.5f;

        // Wet audio buffer: all ones
        juce::AudioSampleBuffer wetAudio(1, NUM_SAMPLES);

        REQUIRE(wetAudio.getNumChannels() == NUM_CHANNELS);
        REQUIRE(wetAudio.getNumSamples() == NUM_SAMPLES);

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            wetAudio.setSample(0, i, 1);
        }

        // Dry audio buffer: all zeros
        juce::AudioSampleBuffer dryAudio(1, NUM_SAMPLES);

        REQUIRE(dryAudio.getNumChannels() == NUM_CHANNELS);
        REQUIRE(dryAudio.getNumSamples() == NUM_SAMPLES);

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            dryAudio.setSample(0, i, 0);
        }

        // Run mixer
        mixer.setWetRatio(WET_RATIO);
        mixer.loadDry(dryAudio);

        mixer.exec(wetAudio);

        CHECK(wetAudio.getNumChannels() == NUM_CHANNELS);
        CHECK(wetAudio.getNumSamples() == NUM_SAMPLES);

        // Since wet = 1 and dry = 0, all values in output buffer should
        // be WET_RATIO
        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            CHECK( compareFloats(wetAudio.getSample(0, i), WET_RATIO) );
        }
    }

}
