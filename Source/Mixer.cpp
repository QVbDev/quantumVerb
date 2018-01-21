/*
  ==============================================================================

    Mixer.cpp
    Created: 20 Jan 2018 5:00:24pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "Mixer.h"

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
