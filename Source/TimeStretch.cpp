/*
  ==============================================================================

    TimeStretch.cpp

  ==============================================================================
*/

#include "TimeStretch.h"
#include "Logger.h"

#include <algorithm>
#include <thread>

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a TimeStretch object associated with an AudioProcessor
     *
     * Creates a TimeStretch object with a handle to a SoundTouch instance
     *
     * @param [in] processor    Pointer to main processor
     *
     * @throws std::runtime_error
     */
    TimeStretch::TimeStretch(juce::AudioProcessor * processor)
        : Task(processor)
    {
        soundtouch.reset(new soundtouch::SoundTouch());
        
        if (!soundtouch)
        {
            logger.dualPrint(Logger::Level::Error, "Failed to create SoundTouch handle");
        }
    }

    //==============================================================================
    /**
     * @brief No parameters to update, do nothing
     *
     * @returns False
     */
    bool TimeStretch::updateParams(const juce::AudioProcessorValueTreeState& params,
                                   const juce::String& blockID)
    {
        // IR length
        auto paramIRLength = params.getRawParameterValue(blockID);
        
        if (!paramIRLength)
        {
            throw std::invalid_argument("Parameter not found for gain in Gain block");
        }

        if (*paramIRLength != irLengthS)
        {
            irLengthS = *paramIRLength;
            mustExec = true;
        }

        return mustExec;
    }

    //==============================================================================
    /**
     * @brief Set sample rate for future IR buffer to process
     *
     * Informs TimeStretch block of original sample rate for IR buffer that wil be
     * received.
     *
     * @param [in] sampleRate   Original sample rate of IR that will be processed
     */
    void TimeStretch::setOrigIRSampleRate(double sampleRate)
    {
        origIRSampleRate = sampleRate;
    }

    //==============================================================================
    /**
     * @brief Apply time stretching algorithm to input IR buffer to change sample rate
     *
     * Stretch or compress input buffer by a factor proportional to original and desired
     * sample rates.
     *
     * @param [in,out] ir   Audio sample buffer to process
     */
    void TimeStretch::exec(juce::AudioSampleBuffer& ir)
    {
        if (!soundtouch)
        {
            logger.dualPrint(Logger::Level::Error, "No SoundTouch instance exists, cannot apply time stretch to IR");
        }

        soundtouch->setChannels(1);
        soundtouch->setSampleRate((unsigned)origIRSampleRate);

        // Move input data to new buffer; ir becomes output buffer for SoundTouch processing
        juce::AudioSampleBuffer irIn = std::move(ir);

        // Calculate tempo change & expected number of samples in output buffer
        if (!origIRSampleRate)
        {
            throw std::invalid_argument("Unknown sample rate for given IR, cannot apply time stretching");
        }

        double newNumSamples = irLengthS * processor->getSampleRate();
        double sampleRateRatio = (double)irIn.getNumSamples() / newNumSamples;

        ir.setSize(irIn.getNumChannels(), (int)std::ceil(newNumSamples), false, true);

        // Use SoundTouch processor to calculate time stretch
        soundtouch->clear();
        soundtouch->setTempo(sampleRateRatio);

        soundtouch->putSamples(irIn.getReadPointer(0), irIn.getNumSamples());

        // Wait for processing to complete
        unsigned curSample = 0;
        unsigned nbSamplesReceived = 0;

        do
        {
            curSample += nbSamplesReceived;

            // Write processed samples to output buffer
            auto curWritePtr = &ir.getWritePointer(0)[curSample];
                
            nbSamplesReceived = soundtouch->receiveSamples(curWritePtr,
                                                            ir.getNumSamples() - curSample);
        }
        while (nbSamplesReceived != 0 &&
                soundtouch->numUnprocessedSamples() != 0);

        // Get last remaining samples from SoundTouch pipeline, if any
        if (soundtouch->numUnprocessedSamples() != 0)
        {
            soundtouch->flush();
            
            curSample += soundtouch->numSamples();
            
            // Write processed samples to output buffer
            auto curWritePtr = &ir.getWritePointer(0)[curSample];

            soundtouch->receiveSamples(curWritePtr, ir.getNumSamples() - curSample);
        }

        // Reset mustExec flag
        mustExec = false;
    }

}
