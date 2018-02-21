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

enum {LOW, HIGH, PEAK};

/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

float measureGain(float * const fft, float freq, float freqRes, int filter) {

	switch (filter) {
	case LOW:
		
		break;

	case HIGH:

		break;

	case PEAK:

		break;

	default:
		return 0;
		break;
	}
}



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

	int numSamples = 44100;

	juce::AudioBuffer<float> sampleBuffer(channelNumber, numSamples);

	float * const buffer = sampleBuffer.getWritePointer(0);

	memset(buffer, 0, sampleBuffer.getNumSamples() * sizeof(*buffer));

	buffer[0] = 1.0f;



	//==============================================================================
	/**
	* FFT Preparation
	*/

	//Prepare a buffer of proper size for FFT

	// compute the next highest power of 2 of sample number
	int order = std::ceil((log(sampleBuffer.getNumSamples()) / log(2)));

	juce::dsp::FFT forwardFFT(order);
	float * fftBuffer = new float[2 * forwardFFT.getSize()];

	memset(fftBuffer, 0, 2*forwardFFT.getSize()*sizeof(*fftBuffer));

	//Compute frequency resolution
	float freqRes = (float)sampleRate / (float)forwardFFT.getSize();


	SECTION("Testing low-shelf filter") {
		reverb::LowShelfFilter filter(&processor, 5000, 0.71, (float)reverb::Filter::invdB(14));
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

	}

	SECTION("Testing high-shelf filter") {
		reverb::HighShelfFilter filter(&processor, 2000, 0.71, (float)reverb::Filter::invdB(10));
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);
	}

	SECTION("Testing peaking filter") {
		reverb::PeakFilter filter(&processor, 5000, 1, (float)reverb::Filter::invdB(10));
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);
	}

	SECTION("Testing filter toggle") {
		reverb::LowShelfFilter filter(&processor, 5000, 0.71, (float)reverb::Filter::invdB(14));
		filter.disable();
		filter.exec(sampleBuffer);

	}

	delete[] fftBuffer;


}
