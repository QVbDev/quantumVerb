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
	constexpr int sampleRate = 88200;
	constexpr int numberChannel = 1;
	constexpr std::chrono::milliseconds audioDurationMs (300);
	const int audioNumSample = (int)std::ceil ((audioDurationMs.count () / 1000.0) * sampleRate);

	// Create Mixer object
	reverb::AudioProcessor processor;

	processor.setPlayConfigDetails (numberChannel, numberChannel,
		sampleRate, audioNumSample);

	REQUIRE (processor.getSampleRate () == sampleRate);
	GainMocked gain (&processor);

	SECTION("Apply gain to audio buffer"){
		constexpr double GAIN_TEST = 2.0;

		// Create audio block

		juce::AudioSampleBuffer audio (1, audioNumSample);

		REQUIRE (audio.getNumChannels () == 1);
		REQUIRE (audio.getNumSamples () == audioNumSample);

		for(int i = 0; i < audioNumSample; i++)
		{
			audio.setSample (0, i, 1);
		}

		float rmsAudio, rmsAudioGain;
		rmsAudio = audio.getRMSLevel (0, 0, audioNumSample);

		// Declare audio to apply gain
		juce::AudioSampleBuffer audioGain (1, audioNumSample);

		REQUIRE (audioGain.getNumChannels () == 1);
		REQUIRE (audioGain.getNumSamples () == audioNumSample);

		for(int i = 0; i < audioNumSample; i++)
		{
			audioGain.setSample (0, i, 1);
		}

		gain.setGainFactor(GAIN_TEST);

        REQUIRE(compareFloats(gain.getGainFactor(), GAIN_TEST));

		//juce::Decibels decibelsToGain(float gain, float gainDb);
		gain.exec (audioGain);
		rmsAudioGain = audioGain.getRMSLevel (0, 0, audioNumSample);


		REQUIRE (rmsAudioGain / rmsAudio == GAIN_TEST);

	}


}
