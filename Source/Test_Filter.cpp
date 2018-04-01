/*
==============================================================================

Test_Filter.cpp

==============================================================================
*/

#include "catch.hpp"

#include "Filter.h"
#include "Test_Util.h"
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"


/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

#define compareFloats(a1, a2) std::abs(a2 - a1) <= 0.01

 // TODO: Test parameter changes

TEST_CASE("Filter class is tested", "[filters]")
{

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
    REQUIRE((int)reverb::Filter::invdB(20) == 10);
    REQUIRE((int)reverb::Filter::invdB(40) == 100);

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


    SECTION("Testing low-shelf filter") 
    {
        float gain = reverb::Filter::invdB(10);
        float freq = 1000;

        reverb::LowShelfFilter filter(&processor, freq, 0.71, gain);
        filter.exec(sampleBuffer);

        memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
        forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

        //Test for gain at low frequency
        CHECK(compareFloats(gain, fftBuffer[0]));

        //Test for gain at high frequency
        CHECK(compareFloats(1, fftBuffer[(int)(forwardFFT.getSize() / 2)]));

        //Test for gain at cut-off frequency
        int cutOffIndex = (int)std::round(freq / freqRes);

        CHECK(compareFloats(fftBuffer[cutOffIndex], std::sqrt(gain)));

        //Test for getAmplitude()
        REQUIRE(compareValues(filter.getAmplitude(freq), std::sqrt(gain)));
        REQUIRE(compareValues(filter.getAmplitude(0), gain));
        REQUIRE(compareValues(filter.getAmplitude(30000), 1.0f));

        //Test for getdBAmplitude()
        REQUIRE(compareValues(filter.getdBAmplitude(freq), reverb::Filter::todB(std::sqrt(gain))));
        REQUIRE(compareValues(filter.getdBAmplitude(0), reverb::Filter::todB(gain)));
        REQUIRE(compareValues(filter.getdBAmplitude(32000), reverb::Filter::todB(1.0f)));
    }

    SECTION("Testing high-shelf filter") 
    {
        float gain = reverb::Filter::invdB(10);
        float freq = 1000;

        reverb::HighShelfFilter filter(&processor, freq, 0.71, gain);
        filter.exec(sampleBuffer);

        memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
        forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

        //Test for gain at low frequency
        CHECK(compareFloats(1, fftBuffer[0]));

        //Test for gain at high frequency
        CHECK(compareFloats(gain, fftBuffer[(int)(forwardFFT.getSize()/2)]));

        //Test for gain at cut-off frequency
        int cutOffIndex = (int)std::round(freq / freqRes);

        CHECK(compareFloats(fftBuffer[cutOffIndex], std::sqrt(gain)));

        //Test for getAmplitude()
        REQUIRE(compareValues(filter.getAmplitude(freq), std::sqrt(gain)));
        REQUIRE(compareValues(filter.getAmplitude(0), 1.0f));
        REQUIRE(compareValues(filter.getAmplitude(30000), gain));
    }

    SECTION("Testing peaking filter") 
    {
        float gain = reverb::Filter::invdB(10);
        float centerFreq = 5000;

        reverb::PeakFilter filter(&processor, centerFreq, 0.71, gain);
        filter.exec(sampleBuffer);

        memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
        forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

        int peakIndex = (int)std::round(centerFreq / freqRes);

        //Test for gain at center frequency
        CHECK(compareFloats(gain, fftBuffer[peakIndex]));

        //Test for gain at both ends of the spectrum
        CHECK(compareFloats(1, fftBuffer[0]));
        CHECK(compareFloats(1, fftBuffer[(int)(forwardFFT.getSize() / 2)]));

        //Test for getAmplitude()
        REQUIRE(compareValues(filter.getAmplitude(centerFreq), gain));
        REQUIRE(compareValues(filter.getAmplitude(0), 1.0f));
    }

    SECTION("Performance_Testing") {
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(30);
        constexpr std::chrono::milliseconds MAX_TOTAL_EXEC_TIME_MS(50);
        std::chrono::milliseconds total_exec(0);        

        // Low shelf
        float gain = reverb::Filter::invdB(14);
        float freq = 1000;
        reverb::LowShelfFilter lowFilter(&processor, freq, 0.71, gain);

        // Measure exec time
        auto start = std::chrono::high_resolution_clock::now();
        lowFilter.exec(sampleBuffer);
        auto end = std::chrono::high_resolution_clock::now();
        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        total_exec += execTime;
        CHECK(execTime.count() < MAX_EXEC_TIME_MS.count());

        // High shelf
        gain = reverb::Filter::invdB(10);
        freq = 20000;
        reverb::HighShelfFilter highFilter(&processor, freq, 0.71, gain);

        // <easure exec time
        start = std::chrono::high_resolution_clock::now();
        highFilter.exec(sampleBuffer);
        end = std::chrono::high_resolution_clock::now();

        execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        total_exec += execTime;
        CHECK(execTime.count() < MAX_EXEC_TIME_MS.count());

        // Peaking 
        gain = reverb::Filter::invdB(10);
        float centerFreq = 10000;
        reverb::PeakFilter peakFilter(&processor, freq, 0.71, gain);

        // Measure exec time
        start = std::chrono::high_resolution_clock::now();
        peakFilter.exec(sampleBuffer);
        end = std::chrono::high_resolution_clock::now();

        execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        total_exec += execTime;
        CHECK(execTime.count() < MAX_EXEC_TIME_MS.count());

        // total low shelf + high shelf + 1 peaking
        CHECK(total_exec.count() < MAX_TOTAL_EXEC_TIME_MS.count());
    }


    delete[] fftBuffer;


}
