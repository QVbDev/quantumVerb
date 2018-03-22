/*
  ==============================================================================

    Mixer.cpp
    Created: 20 Jan 2018 5:00:24pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "Mixer.h"

#include "Logger.h"

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

    /**
     * @brief Updates parameters from processor parameter tree
     * 
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void Mixer::updateParams(const juce::AudioProcessorValueTreeState& params,
                             const juce::String& blockId)
    {
        float _wetRatio = getParam(params, blockId);

        if (wetRatio != _wetRatio)
        {
            wetRatio = _wetRatio;
            mustExec = true;
        }
    }

    //==============================================================================
    /**
     * @brief Mix the wet and dry sound according to a proportionality parameter
     *
     * Mixing the wet sound passed as an argument with the dry sound content in the 
	 dryAudio buffer.
     *
     * @param [in,out] wetAudio Buffer containing the wet audio signal 
     */
    AudioBlock Mixer::exec(AudioBlock wetAudio)
    {
        wetAudio.multiply(wetRatio);
        wetAudio.addWithMultiply(dryAudioCopy, 1 - wetRatio);

        // Reset mustExec flag
        mustExec = false;

        return wetAudio;
    }

    //==============================================================================
    /**
    * @brief loads the dry signal into the dryAudio variable
    *
    * @param [in,out] dryAudio Buffer containing the dry signal
    */
    void Mixer::loadDry(AudioBlock dryAudio)
    {
		dryAudioCopy.clear();
        dryAudioCopy.setSize(1, (int)dryAudio.getNumSamples());

        dryAudioCopy.copyFrom(0, 0,
                              dryAudio.getChannelPointer(0),
                              (int)dryAudio.getNumSamples());
    }

}
