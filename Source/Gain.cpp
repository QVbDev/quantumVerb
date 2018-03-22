/*
  ==============================================================================

    Gain.cpp

  ==============================================================================
*/

#include "Gain.h"

#include "Logger.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructor. Creates a Gain object
     *
     * Creates a Gain object associated to an AudioProcessor.
     *
     * @param [in] processor    Pointer to main processor
     */
    Gain::Gain(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void Gain::updateParams(const juce::AudioProcessorValueTreeState& params,
                            const juce::String& blockId)
    {
        float _gainFactor = getParam(params, blockId);

        if (gainFactor != _gainFactor)
        {
            gainFactor = _gainFactor;
            mustExec = true;
        }
    }

    //==============================================================================
    /**
     * @brief Apply Gain to input buffer to change volume of signal audio 
     *
     * Gain to apply to audio buffer is stocked in gainFactor
     *
     * @param [in,out] buffer   Audio sample buffer to process
     */
    AudioBlock Gain::exec(AudioBlock buffer)
    {
		buffer.multiply(gainFactor);

        // Reset mustExec flag
        mustExec = false;

        return buffer;
    }

}
