/*
  ==============================================================================

    Convolution.cpp

  ==============================================================================
*/

#include "Convolution.h"

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
    Convolution::Convolution(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }

    //==============================================================================
    /**
     * @brief No parameters to update, do nothing
     *
     * @returns False
     */
    bool Convolution::updateParams(const juce::AudioProcessorValueTreeState&,
                                   const juce::String&)
    {
        return false;
    }

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in,out] audio    (TODO) Parameter description
     */
    void Convolution::exec(juce::AudioSampleBuffer& audio)
    {
    }

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    *
    * @param [in] ir    Parameter description
    */
    void Convolution::loadIR(juce::AudioSampleBuffer& ir)
    {
    }

}