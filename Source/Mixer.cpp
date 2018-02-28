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
     * @brief Constructor. Creates a Mixer object. 
     *
     * Creates a Mixer object associated to an AudioProcessor.
     *
     * @param [in] processor    Pointer to main processor
     */
    Mixer::Mixer(juce::AudioProcessor * processor)
        : Task(processor)
    { 
		wetRatio = 0.0;
    }
    
    //==============================================================================
    /**
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool Mixer::updateParams(const juce::AudioProcessorValueTreeState& params,
                             const juce::String& blockId)
    {
        bool changedConfig = false;

        // Dry/wet ratio
        auto paramWetRatio = params.getRawParameterValue(blockId);

        if (!paramWetRatio)
        {
            throw std::invalid_argument("Parameter not found for wet ratio in Mixer block");
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
	* @brief Destroys a Mixer object

	*/
	Mixer::~Mixer()
	{
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
    void Mixer::exec(juce::AudioSampleBuffer& wetAudio)
    {
		wetAudio.applyGain (wetRatio);
		dryAudio.applyGain (1 - wetRatio);
		wetAudio.addFrom(0,0,dryAudio,0,0,wetAudio.getNumSamples(),1.0);

    }

    //==============================================================================
    /**
    * @brief loads the dry signal into the dryAudio variable
    *
    * @param [in,out] dryAudio Buffer containing the dry signal
    */
    void Mixer::loadDry(const juce::AudioSampleBuffer dryAudio)
    {
		this->dryAudio = dryAudio;
    }

}
