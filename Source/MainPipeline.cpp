/*
  ==============================================================================

    MainPipeline.cpp
    Created: 20 Jan 2018 4:57:18pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "MainPipeline.h"

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
    MainPipeline::MainPipeline(juce::AudioProcessor * processor)
        : Task(processor)
    {
        // Initialise pipeline steps
        convolution = std::make_shared<Convolution>(processor);
        gain = std::make_shared<Gain>(processor);
        dryWetMixer = std::make_shared<Mixer>(processor);
    }

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in,out] audio    (TODO) Parameter description
     */
    void MainPipeline::exec(juce::AudioSampleBuffer& audio)
    {
    }

    //==============================================================================
    /**
     * @brief Brief description
     *
     * Detailed description
     *
     * @param [in] ir   Parameter description
     */
    void MainPipeline::loadIR(juce::AudioSampleBuffer& ir)
    {
    }

}
