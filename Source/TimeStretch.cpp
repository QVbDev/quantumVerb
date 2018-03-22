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
     * @brief Apply time stretching algorithm to input IR buffer to change sample rate
     * 
     * Stretch or compress input buffer by a factor proportional to original and desired
     * sample rates.
     *
     * NOTE: prepareIR() method should be called before to manage buffer size. This
     *       will copy the original IR and resize the given buffer to the appropriate
     *       size based on sample rate and desired length.
     *
     * @param [in,out] ir   Audio sample buffer to process
     */
    AudioBlock TimeStretch::exec(AudioBlock ir)
    {
        if (!soundtouch)
        {
            logger.dualPrint(Logger::Level::Error, "No SoundTouch instance exists, cannot apply time stretch to IR");
            return ir;
        }

        soundtouch->setChannels(1);
        soundtouch->setSampleRate((unsigned)sampleRate);

        // Calculate tempo change & expected number of samples in output buffer
        int newNumSamples = getOutputNumSamples();
        double sampleRateRatio = (double)irOrig.getNumSamples() / (double)newNumSamples;

        // Use SoundTouch processor to calculate time stretch
        soundtouch->clear();
        soundtouch->setTempo(sampleRateRatio);

        soundtouch->putSamples(irOrig.getReadPointer(0), irOrig.getNumSamples());

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
        while (nbSamplesReceived != 0);

        // Get last remaining samples from SoundTouch pipeline, if any
        soundtouch->flush();
        do
        {
            auto curWritePtr = &ir.getChannelPointer(0)[curSample];

            nbSamplesReceived = soundtouch->receiveSamples(curWritePtr,
                                                           (unsigned)ir.getNumSamples() - curSample);

            curSample += nbSamplesReceived;
        }
        while (nbSamplesReceived != 0);

        jassert(curSample == ir.getNumSamples());

        // Reset mustExec flag
        mustExec = false;

        return ir;
    }

    //==============================================================================
    /**
     * @brief Copies given IR to internal representation and resizes it before processing
     *
     * @param [in,out] ir   IR to copy and resize
     */
    void TimeStretch::prepareIR(juce::AudioSampleBuffer& ir)
    {
        irOrig.makeCopyOf(ir);
        ir.setSize(ir.getNumChannels(), getOutputNumSamples());
    }

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
