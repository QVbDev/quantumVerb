/*
  ==============================================================================

    Mixer.cpp
    Created     : 20 Jan 2018 5:00:24pm by Eric Seguin
    implemented : 8 Fev 2018  23:40:01  by Ilham Ennaji  

  ==============================================================================
*/

#include "Mixer.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructor. Creates a Mixer object. 
     *
     * Creates a Mixer object associated to an AudioProcessor.
     *
     * @param [in] processor    Pointer to main processor
     */
    Mixer::Mixer(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }

    //==============================================================================
    /**
     * @brief Mix the wet and dry sound according to a proportionality parameter
     *
     * Mixing the wet sound passed as an argument with the dry sound content
     * in the dryAudio buffer.
     * @param [in,out] wetAudio Buffer containing the wet audio signal 
     */
    void Mixer::exec(juce::AudioSampleBuffer& wetAudio)
    {
		wetAudio = wetRatio * wetAudio + (1 - wetRatio) * dryAudio;
    }

    //==============================================================================
    /**
    * @brief loads the dry signal into the dryAudio variable
    *
    * @param [in,out] dryAudio Buffer containing the dry signal
    */
    void Mixer::loadDry(const juce::AudioSampleBuffer& dryAudio)
    {
		this.dryAudio = dryAudio;
    }

}
