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
        dryWetMixer->loadDry(audio);
        convolution->exec(audio);
        dryWetMixer->exec(audio);
        gain->exec(audio);
    }

    //==============================================================================
    /**
     * @brief Move IR buffer into MainPipeline object
     *
     * @param [in] irIn Input IR buffer, empty after this method is called
     */
    void MainPipeline::loadIR(juce::AudioSampleBuffer&& irIn)
    {
        ir = std::move(irIn);
		convolution->loadIR(ir);
    }

}
