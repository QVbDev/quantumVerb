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

TEST_CASE("Filter class is tested", "[filters]") {

	//==============================================================================
	/**
	* Processor preparation
	*/
	constexpr int sampleRate = 44100;
	constexpr int channelNumber = 1;
	constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
	constexpr double NUM_SAMPLES_PER_BLOCK = (BLOCK_DURATION_MS.count() / 1000.0) * sampleRate;


	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails(channelNumber, channelNumber, sampleRate, std::ceil(NUM_SAMPLES_PER_BLOCK));

	REQUIRE(processor.getSampleRate() == sampleRate);

	//Test invdB function
	REQUIRE((int)reverb::Filter::invdB(0) == 1);
	REQUIRE((int)reverb::Filter::invdB(10) == 10);
	REQUIRE((int)reverb::Filter::invdB(20) == 100);

	//==============================================================================
	/**
	* Unit impulse construction
	*/

	juce::AudioBuffer<float> sampleBuffer(channelNumber, 40000);

	float * const buffer = sampleBuffer.getWritePointer(0);

	for (int i = 0; i < sampleBuffer.getNumSamples(); i++) {

		if (i == 0) {
			buffer[i] = 0.125f;
		}
		else {
			buffer[i] = 0;
		}
	}

	//==============================================================================
	/**
	* FFT Preparation
	*/

	//Prepare a buffer of proper size for FFT

	// compute the next highest power of 2 of sample number
	int order = ceil((log(sampleBuffer.getNumSamples()) / log(2)));

	juce::dsp::FFT forwardFFT(order);

	const int fftSize = pow(2, order);
	float * fftBuffer = new float[2 * fftSize];

	memset(fftBuffer, 0, sizeof(*fftBuffer));

	SECTION("Testing low-shelf filter") {
		reverb::LowShelfFilter lowShelf(&processor, 1000, 0.71, (float)reverb::Filter::invdB(10));
		lowShelf.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

	}

	SECTION("Testing high-shelf filter") {
		reverb::HighShelfFilter lowShelf(&processor, 100, 0.71, (float)reverb::Filter::invdB(-24));
		lowShelf.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);
	}

	SECTION("Testing peaking filter") {
		reverb::PeakFilter lowShelf(&processor, 100, 0.71, (float)reverb::Filter::invdB(-24));
		lowShelf.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);
	}

	delete[] fftBuffer;


}
