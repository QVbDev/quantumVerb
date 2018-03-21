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
    
    //==============================================================================
    /**
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool Gain::updateParams(const juce::AudioProcessorValueTreeState& params,
                            const juce::String& blockId)
    {
        // Gain factor
        auto paramGain = params.getRawParameterValue(blockId);

        if (!paramGain)
        {
            throw std::invalid_argument("Parameter not found for gain in Gain block");
        }

        if (*paramGain != gainFactor)
        {
            gainFactor = *paramGain;
            mustExec = true;
        }

        return mustExec;
    }

    //==============================================================================
    /**
     * @brief Apply Gain to input buffer to change volume of signal audio 
     *
     * Gain to apply to audio buffer is stocked in gainFactor
     *
     * @param [in,out] buffer   Audio sample buffer to process
     */
    void Gain::exec(juce::AudioSampleBuffer& buffer)
    {
        if (mustExec)
        {
            buffer.applyGain(gainFactor);

            // Reset mustExec flag
            mustExec = false;
        }
    }

}
