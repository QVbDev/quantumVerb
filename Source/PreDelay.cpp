/*
  ==============================================================================

    PreDelay.cpp
    Created: 14 Feb 2018 3:26:12pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "PreDelay.h"

#include "Logger.h"

#include <algorithm>

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a PreDelay object associated with an AudioProcessor
     *
     * @param [in] processor    Pointer to main processor
     */
    PreDelay::PreDelay(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void PreDelay::updateParams(const juce::AudioProcessorValueTreeState& params,
                                const juce::String& blockId)
    {
        float _delayMs = getParam(params, blockId);

        if (delayMs != _delayMs)
        {
            delayMs = _delayMs;
            mustExec = true;
        }
    }
    
    //==============================================================================
    /**
     * @brief Applies predelay to impulse response
     *
     * Zero-pads beginning of impulse response buffer to match requested predelay.
     * Expects given audio block to be large enough to accomodate predelay.
     *
     * @param [in,out] ir   Impulse response to modify
     */
    AudioBlock PreDelay::exec(AudioBlock ir)
    {
        const size_t numSamplesToAdd = getNumSamplesToAdd();
        const size_t lengthWithoutPreDelay = (ir.getNumSamples() - numSamplesToAdd);

        if (numSamplesToAdd > 0)
        {
            // Shift audio samples in memory to leave room for predelay
            // NB: Use memmove instead of memcpy since dst and src will probably overlap
            float * irPtr = ir.getChannelPointer(0);
            memmove(irPtr + numSamplesToAdd, irPtr, lengthWithoutPreDelay * sizeof(float));

            // Clear predelay samples
            memset(irPtr, 0, numSamplesToAdd * sizeof(float));
        }

        // Reset mustExec flag
        mustExec = false;

        return ir;
    }

    //==============================================================================
    /**
     * @brief Resizes given IR before processing
     *
     * @param [in,out] ir   IR to prepare
     */
    void PreDelay::prepareIR(juce::AudioSampleBuffer& ir)
    {
        ir.setSize(ir.getNumChannels(),
                   ir.getNumSamples() + getNumSamplesToAdd(),
                   true, false, false);
    }

    /**
     * @brief Returns expected number of samples after processing
     *
     * @param [in] inputNumSamples  Number of samples in input buffer
     */
    int PreDelay::getNumSamplesToAdd()
    {
        return (int)std::ceil(sampleRate * (delayMs / 1000.0));
    }

}
