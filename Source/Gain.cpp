/*
  ==============================================================================

    Gain.cpp

  ==============================================================================
*/

#include "Gain.h"
#include "PluginParameters.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
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
    bool Gain::updateParams(const std::string& blockId)
    {
        auto& params = getMapOfParams();
        bool changedConfig = false;

        // Gain factor
        auto paramGain = dynamic_cast<juce::AudioParameterFloat*>(params.at(blockId));

        if (!paramGain)
        {
            throw std::invalid_argument("Received non-float parameter for gain in Gain block");
        }

        if (*paramGain != gainFactor)
        {
            gainFactor = *paramGain;
            changedConfig = true;
        }

        return changedConfig;
    }

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in,out] buffer   (TODO) Parameter description
     */
    void Gain::exec(juce::AudioSampleBuffer& buffer)
    {
    }

}
