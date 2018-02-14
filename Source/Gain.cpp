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
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in] processor    Pointer to main processor
     */
    Gain::Gain(juce::AudioProcessor * processor)
        : Task(processor)
    {
        // random testing value
        gainFactor = 0.03;
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
