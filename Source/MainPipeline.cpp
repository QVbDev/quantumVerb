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

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void MainPipeline::updateParams(const juce::AudioProcessorValueTreeState& params,
                                    const juce::String&)
    {
        gain->updateParams(params, AudioProcessor::PID_AUDIO_OUT_GAIN);
        dryWetMixer->updateParams(params, AudioProcessor::PID_WETRATIO);
    }

    //==============================================================================
    /**
     * @brief Update sample rate for pipeline and child tasks
     * 
     * Compares new sample rate with previous value. If different, sets mustExec to
     * true in order to re-run pipeline for new sample rate. Store new sample rate
     * value in object.
     *
     * @param [in] sr   Sample rate
     */
    void MainPipeline::updateSampleRate(double sr)
    {
        if (sr != sampleRate)
        {
            sampleRate = sr;
            mustExec = true;

            convolution->updateSampleRate(sr);
            gain->updateSampleRate(sr);
            dryWetMixer->updateSampleRate(sr);
        }
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
    AudioBlock MainPipeline::exec(AudioBlock audio)
    {
        dryWetMixer->loadDry(audio);

        convolution->exec(audio);
        dryWetMixer->exec(audio);
        gain->exec(audio);

        return audio;
    }

    //==============================================================================
    /**
     * @brief Copy reference to IR buffer
     *
     * Calls convolution block's loadIR method to pass along the given audio block
     *
     * @param [in] irIn Input IR block
     */
    void MainPipeline::loadIR(AudioBlock irIn)
    {
        ir = irIn;
		convolution->loadIR(irIn);
    }

}
