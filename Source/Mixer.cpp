/*
  ==============================================================================

    Mixer.cpp
    Created: 20 Jan 2018 5:00:24pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "Mixer.h"
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
    Mixer::Mixer(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }
    
    //==============================================================================
    /**
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool Mixer::updateParams(const std::string& blockId)
    {
        auto& params = getMapOfParams();
        bool changedConfig = false;

        // Dry/wet ratio
        auto paramWetRatio = dynamic_cast<juce::AudioParameterFloat*>(params.at(blockId));

        if (!paramWetRatio)
        {
            throw std::invalid_argument("Received non-float parameter for wet ratio in Mixer block");
        }

        if (*paramWetRatio != wetRatio)
        {
            wetRatio = *paramWetRatio;
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
     * @param [in,out] wetAudio (TODO) Parameter description
     */
    void Mixer::exec(juce::AudioSampleBuffer& wetAudio)
    {
    }

    //==============================================================================
    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    *
    * @param [in,out] dryAudio (TODO) Parameter description
    */
    void Mixer::loadDry(const juce::AudioSampleBuffer& dryAudio)
    {
    }

}
