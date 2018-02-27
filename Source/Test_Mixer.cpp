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

TEST_CASE("Mixer class is tested", "[Mixer]") {

	constexpr int sampleRate = 88200;
	constexpr int numberChannel = 1;
	constexpr std::chrono::milliseconds audioDurationMs (300);
	const int audioNumSample = (int)std::ceil ((audioDurationMs.count () / 1000.0) * sampleRate);
	// Create Mixer object
	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails (numberChannel, numberChannel,
		sampleRate, audioNumSample);

	REQUIRE (processor.getSampleRate () == sampleRate);
	reverb::Mixer mixerObj (&processor);

	SECTION ("Mix two audio buffer") {
		constexpr double RATIO_WET = 0.5;

		// Create one audio block 
		juce::AudioSampleBuffer wetAudio (1, audioNumSample);

		REQUIRE (wetAudio.getNumChannels () == 1);
		REQUIRE (wetAudio.getNumSamples () == audioNumSample);

		for(int i = 0; i < audioNumSample; i++)
		{
			wetAudio.setSample (0, i, 1);
		}

		// Create other audio block 
		juce::AudioSampleBuffer dryAudio (1, audioNumSample);

		REQUIRE (dryAudio.getNumChannels () == 1);
		REQUIRE (dryAudio.getNumSamples () == audioNumSample);
		
		for(int i = 0; i < audioNumSample; i++)
		{
			dryAudio.setSample (0, i, 1);
		}

		mixerObj.wetRatio = RATIO_WET;

		// Mixing
		mixerObj.loadDry (dryAudio);
		mixerObj.exec (wetAudio);

		// Test 

		juce::AudioSampleBuffer wetAudioT (1, audioNumSample);

		REQUIRE (wetAudioT.getNumChannels () == 1);
		REQUIRE (wetAudioT.getNumSamples () == audioNumSample);

		for(int i = 0; i < audioNumSample; i++)
		{
			wetAudioT.setSample (0, i, 1);
		}

		wetAudioT.applyGain (RATIO_WET);
		dryAudio.applyGain (1 - RATIO_WET);
		wetAudioT.addFrom (0, 0, dryAudio, 0, 0, wetAudioT.getNumSamples (), 1.0);

		for(int i = 1; i < audioNumSample; i++)
		{
			REQUIRE ((wetAudioT.getSample (0, i)/wetAudio.getSample (0, i))== 1);
		}

	}



}
