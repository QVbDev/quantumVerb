/*
  ==============================================================================

    MainPipeline.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include "Convolution.h"
#include "Gain.h"
#include "Mixer.h"

namespace reverb
{

    //==============================================================================
    /**
     * Main audio processing pipeline. Contains various steps applied on every processing cycle,
     * in particular convolution with the associated impulse response and some cleanup steps
     * afterwards. This pipeline constitutes the critical path (i.e. it limits plugin
     * performance) and thus is more heavily optimised than its IR counterpart.
     */
    class MainPipeline : public Task
    {
    public:
        //==============================================================================
        MainPipeline(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<MainPipeline>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& audio) override;

        //==============================================================================
        void loadIR(juce::AudioSampleBuffer&& ir);

        //==============================================================================
        Convolution::Ptr convolution;
        Gain::Ptr gain;
        Mixer::Ptr dryWetMixer;

    protected:
        juce::AudioSampleBuffer ir;
    };

}
