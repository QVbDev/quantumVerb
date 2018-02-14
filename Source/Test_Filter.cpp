/*
  ==============================================================================

    Test_Filter.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "Filter.h"
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

//TEST_CASE("(TODO) Test case description", "[Filter]") {
//}

TEST_CASE("Filters are tested", "[filters]") {

	constexpr int sampleRate = 44100;
	constexpr int channelNumber = 1;
	constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
	constexpr double NUM_SAMPLES_PER_BLOCK = (BLOCK_DURATION_MS.count() / 1000.0) * sampleRate;


	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails(channelNumber, channelNumber, sampleRate, std::ceil(NUM_SAMPLES_PER_BLOCK));

	REQUIRE(processor.getSampleRate() == sampleRate);

	 

	SECTION("Using filters on unit impulse") {
		//Unit impulse construction
		juce::AudioBuffer<float> bufferToFill(channelNumber, sampleRate * 2);

		float * const buffer = bufferToFill.getWritePointer(0);

		for (int i = 0; i < bufferToFill.getNumSamples(); i++) {

			if (i == 22050)
				buffer[i] = 1.0f;
			else
				buffer[i] = 0;
		}

		reverb::LowShelfFilter lowShelf(&processor, 100, 0.71, reverb::invdB(-24));
		reverb::HighShelfFilter highShelf(&processor, 100, 0.71, reverb::invdB(-24));
		reverb::PeakFilter peakFilter(&processor, 100, 0.71, reverb::invdB(-24));

	}

	




}
