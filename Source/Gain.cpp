/*
  ==============================================================================

    Gain.cpp

  ==============================================================================
*/

#include "Gain.h"

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
     * @brief Apply Gain to input buffer to change volume of signal audio 
     *
     * Gain to apply to audio buffer is stocked in gainFactor
     *
     * @param [in,out] buffer   Audio sample buffer to process
     */
    void Gain::exec(juce::AudioSampleBuffer& buffer)
    {
		// Accepted value of gainFactor

		if(gainFactor > 0 && gainFactor < 10) { 
			buffer.applyGain (gainFactor); }

		else {
			 buffer.applyGain(1.0);
		}
		
    }

}
