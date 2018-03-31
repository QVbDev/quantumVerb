/*
==============================================================================

Test_Equalizer.cpp

==============================================================================
*/

#include "catch.hpp"

#include "Equalizer.h"
#include "Test_Util.h"
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"


/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

namespace reverb {
    class EqualizerMocked : public Equalizer 
    
    {
    public:

        using Equalizer::Equalizer;

        void setFrequency(float freq, int id) {
            filterSet[id]->frequency= freq;
        }

        void setGain(float gain, int id) {
            EQGains[id] = gain;
        }

        void setQ(float q, int id) {
            filterSet[id]->Q = q;
        }

        float getFrequency(float freq, int id) {
            return filterSet[id]->frequency;
        }

        float getGain(float gain, int id) {
            return filterSet[id]->gainFactor;
        }

        float getQ(float q, int id) {
            return filterSet[id]->Q;
        }
    };
}


TEST_CASE("Equalizer class is tested", "[equalizer]")
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

    memset(fftBuffer, 0, 2 * forwardFFT.getSize() * sizeof(*fftBuffer));

    //Compute frequency resolution
    float freqRes = (float)sampleRate / (float)forwardFFT.getSize();

    float * dBPlot = new float[numSamples];


    /*SECTION("Testing equalizer")
    {

        reverb::Equalizer EQ(&processor);

        EQ.exec(sampleBuffer);

        memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
        forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

        for (int i = 0; i < numSamples; i++) {
            dBPlot[i] = reverb::Filter::todB(fftBuffer[i]);
        }

        //Test band gains (at 0, f1, f2 and fmax)

        REQUIRE(compareValues(fftBuffer[0], EQ.getEQGain(0)));

        REQUIRE(compareValues(fftBuffer[(int)(EQ.getFilterFrequency(1) / freqRes)], EQ.getEQGain(1)));

        REQUIRE(compareValues(fftBuffer[(int)(EQ.getFilterFrequency(2) / freqRes)], EQ.getEQGain(2)));

        REQUIRE(compareValues(fftBuffer[(int)(21000 / freqRes)], EQ.getEQGain(3)));

        //Test getdBAmplitude()

        REQUIRE(compareValues(reverb::Filter::todB(fftBuffer[(int)(1000 / freqRes)]), EQ.getdBAmplitude(1000)));

    }*/

    SECTION("Testing equalizer")
    {
        reverb::EqualizerMocked EQ(&processor);

        /*-----------------------------------------Testing borderline case---------------------------------------*/

        //Set low-shelf filter at maximum frequency, maximum gain and minimum Q
        EQ.setFrequency(500, 0);
        EQ.setGain(5.2, 0);
        EQ.setQ(0.71, 0);

        //Set first peak filter to minimum gain, minimum Q as close as can be to peak 2
        EQ.setFrequency(1500, 1);
        EQ.setGain(0.06, 1);
        EQ.setQ(0.71, 1);

        //Set second peak filter to maximum gain, minimum Q as close as can be to peak 1
        EQ.setFrequency(2600, 2);
        EQ.setGain(5.2, 2);
        EQ.setQ(0.71, 2);

        //Set high-shelf filter at minimum frequency, minimum gain and minimum Q
        EQ.setFrequency(8000, 3);
        EQ.setGain(0.06, 3);
        EQ.setQ(0.71, 3);

        bool converged = true;
        try
        {
            EQ.calibrateFilters();
        }
        catch (const std::exception & e)
        {
            converged = false;
        }

        REQUIRE(converged);

        EQ.exec(sampleBuffer);

        memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
        forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);

        //Set breakpoint here to observe FFT in fftBuffer

        /*----------------------------------------------Convergence test----------------------------------------------*/

        converged = true;
        int i = 0;
        int frequency = 2600;

        //Check at which frequency the peak filters are too close for the calibration to converge (loop capped at 200)

        while (converged && i < 200) 
        {
            frequency -= 10;
            EQ.setFrequency(frequency, 2);

            try
            {
                EQ.calibrateFilters();
            }
            catch (const std::exception & e)
            {
                converged = false;
            }

            i++;
        }

        //Check if margin is large enough
        REQUIRE((2600.0f / (float)frequency) > 1.3);



    }

    delete[] fftBuffer;
}