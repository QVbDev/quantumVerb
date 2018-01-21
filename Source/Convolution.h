/*
  ==============================================================================

    Convolution.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
     * TODO: Description
     */
    class Convolution : public Task,
                        public juce::dsp::Convolution
    {
    public:
        //==============================================================================
        Convolution(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<Convolution>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& audio) override;

        //==============================================================================
        void loadIR(juce::AudioSampleBuffer& ir);
    };

}