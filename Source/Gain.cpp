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
     * @brief Constructor. Creates a Gain object.
     * Creates a Gain object associated to an AudioProcessor.
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
     *  Gain to apply to audio buffer is stocked in gainFactor
     *
     * @param [in,out] Audio sample buffer to process
     */
    void Gain::exec(juce::AudioSampleBuffer& buffer)
    {
		buffer.applyGain(gainFactor);
    }

}
