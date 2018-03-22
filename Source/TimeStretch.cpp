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

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void TimeStretch::updateParams(const juce::AudioProcessorValueTreeState& params,
                                   const juce::String& blockId)
    {
        float _irLengthS = getParam(params, blockId);

        if (_irLengthS != irLengthS)
        {
            irLengthS = _irLengthS;
            mustExec = true;
        }
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
    void TimeStretch::setOrigIRSampleRate(double sr)
    {
        origIRSampleRate = sr;
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
    AudioBlock TimeStretch::exec(AudioBlock ir)
    {
        if (!soundtouch)
        {
            logger.dualPrint(Logger::Level::Error, "No SoundTouch instance exists, cannot apply time stretch to IR");
        }

        soundtouch->setChannels(1);
        soundtouch->setSampleRate((unsigned)origIRSampleRate);

        // Copy input data to new buffer; ir becomes output buffer for SoundTouch processing
        juce::AudioSampleBuffer irCopy(1, (int)ir.getNumSamples());
        irCopy.copyFrom(0, 0, ir.getChannelPointer(0), (int)ir.getNumSamples());

        // Calculate tempo change & expected number of samples in output buffer
        if (!origIRSampleRate)
        {
            throw std::invalid_argument("Unknown sample rate for given IR, cannot apply time stretching");
        }

        double newNumSamples = getOutputNumSamples();
        double sampleRateRatio = (double)irCopy.getNumSamples() / newNumSamples;

        // Use SoundTouch processor to calculate time stretch
        soundtouch->clear();
        soundtouch->setTempo(sampleRateRatio);

        soundtouch->putSamples(irCopy.getReadPointer(0), irCopy.getNumSamples());

        // Wait for processing to complete
        unsigned curSample = 0;
        unsigned nbSamplesReceived = 0;

        do
        {
            curSample += nbSamplesReceived;

            // Write processed samples to output buffer
            auto curWritePtr = &ir.getChannelPointer(0)[curSample];
                
            nbSamplesReceived = soundtouch->receiveSamples(curWritePtr,
                                                           (unsigned)ir.getNumSamples() - curSample);
        }
        while (nbSamplesReceived != 0 &&
               soundtouch->numUnprocessedSamples() != 0);

        // Get last remaining samples from SoundTouch pipeline, if any
        if (soundtouch->numUnprocessedSamples() != 0)
        {
            soundtouch->flush();
            
            curSample += soundtouch->numSamples();
            
            // Write processed samples to output buffer
            auto curWritePtr = &ir.getChannelPointer(0)[curSample];

            soundtouch->receiveSamples(curWritePtr, (unsigned)ir.getNumSamples() - curSample);
        }

        // Reset mustExec flag
        mustExec = false;

        return ir;
    }

    //==============================================================================
    /**
     * @brief Returns expected number of samples after processing
     *
     * @param [in] inputNumSamples  Number of samples in input buffer
     */
    int TimeStretch::getOutputNumSamples()
    {
        return (int)std::ceil(irLengthS * sampleRate);
    }

}
