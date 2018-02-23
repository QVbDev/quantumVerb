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

		// Mixing
		mixerObj.loadDry (dryAudio);
		mixerObj.exec (wetAudio);


	}



}
