/*
  ==============================================================================

    MainPipeline.h
    Created: 20 Jan 2018 4:57:18pm
    Author:  Eric Seguin

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
    * TODO: Description
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
        void loadIR(juce::AudioSampleBuffer& ir);

        //==============================================================================
        Convolution::Ptr convolution;
        Gain::Ptr gain;
        Mixer::Ptr dryWetMixer;

    protected:
        juce::AudioSampleBuffer ir;
    };

}
