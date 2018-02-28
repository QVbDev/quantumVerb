/*
==============================================================================

Test_Filter.cpp

==============================================================================
*/

#include "catch.hpp"

#include "Filter.h"
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"


/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/


bool compareValues(const float a1, const float a2) {
	float ratio = a1 / a2;

	if (ratio >= 0.999 && ratio <= 1.001)
		return true;

	else
		return false;

}

#define compareFloats(a1, a2) std::abs(a2 - a1) <= 0.01


 // TODO: Test parameter changes

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
		float gain = reverb::Filter::invdB(14);
		float freq = 5000;

		reverb::LowShelfFilter filter(&processor, freq, 0.71, gain);
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

		//Test for gain at low freuquency
		REQUIRE(compareFloats(gain, fftBuffer[0]));

		//Test for gain at high frequency
		REQUIRE(compareFloats(1, fftBuffer[(int)(forwardFFT.getSize() / 2)]));

		//Test for cut-off frequency
		int cutOffIndex = (int)std::round(freq / freqRes);

		REQUIRE(compareFloats(fftBuffer[cutOffIndex], std::sqrt(gain)));
	}

	SECTION("Testing high-shelf filter") {
		float gain = reverb::Filter::invdB(10);
		float freq = 20000;

		reverb::HighShelfFilter filter(&processor, freq, 0.71, gain);
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

		//Test for gain at low frequency
		REQUIRE(compareFloats(1, fftBuffer[0]));

		//Test for gain at high frequency
		REQUIRE(compareFloats(gain, fftBuffer[(int)(forwardFFT.getSize()/2)]));

		//Test for cut-off frequency
		int cutOffIndex = (int)std::round(freq / freqRes);

		REQUIRE(compareFloats(fftBuffer[cutOffIndex], std::sqrt(gain)));
	}

	SECTION("Testing peaking filter") {
		float gain = reverb::Filter::invdB(10);
		float centerFreq = 5000;

		reverb::PeakFilter filter(&processor, centerFreq, 0.71, gain);
		filter.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

		int peakIndex = (int)std::round(centerFreq / freqRes);

		//Test for gain at center frequency
		REQUIRE(compareFloats(gain, fftBuffer[peakIndex]));

		//Test for gain at both ends of the spectrum
		REQUIRE(compareFloats(1, fftBuffer[0]));
		REQUIRE(compareFloats(1, fftBuffer[(int)(forwardFFT.getSize() / 2)]));
	}

    /*
	SECTION("Testing filter toggle") {
		reverb::LowShelfFilter filter(&processor, 5000, 0.71, (float)reverb::Filter::invdB(14));
		filter.disable();
		filter.exec(sampleBuffer);

	}
    */
	delete[] fftBuffer;


}
