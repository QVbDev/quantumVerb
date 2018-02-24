/*
  ==============================================================================

    Test_Gain.cpp

  ==============================================================================
*/

#include "catch.hpp"
#include "PluginProcessor.h"
#include "Gain.h"
#include <chrono>


/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

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
	reverb::Gain gain (&processor);

	SECTION("Apply gain to audio buffer"){

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

		float gain1 = 2.0f, gain2 =100.0f ;
		//juce::Decibels decibelsToGain(float gain, float gainDb);
		audioGain.applyGain (gain1);
		rmsAudioGain = audioGain.getRMSLevel (0, 0, audioNumSample);

		REQUIRE (rmsAudioGain/rmsAudio == 2.0f);


	}


}
