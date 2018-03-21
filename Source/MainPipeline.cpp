/*
  ==============================================================================

    MainPipeline.cpp
    Created: 20 Jan 2018 4:57:18pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "MainPipeline.h"
#include "PluginProcessor.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a MainPipeline object associated with an AudioProcessor
     *
     * Creates a MainPipeline and each of its steps.
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
     * @brief Read processor parameters and update child parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool MainPipeline::updateParams(const juce::AudioProcessorValueTreeState& params,
                                    const juce::String&)
    {
        bool changedParams = false;

        // Update child parameters
        changedParams |= gain->updateParams(params, AudioProcessor::PID_AUDIO_OUT_GAIN);
        changedParams |= dryWetMixer->updateParams(params, AudioProcessor::PID_WETRATIO);

        return changedParams;
    }

    //==============================================================================
    /**
     * @brief Apply reverb effect to given audio buffer
     *
     * Load original audio buffer into dryWetMixer object, then apply reverb pipeline
     * steps in series (convolution w/ IR, dry/wet mixing and output attenuation). Output
     * replaces samples in given audio buffer.
     *
     * @param [in,out] audio    Audio sample buffer
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
