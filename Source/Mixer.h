/*
  ==============================================================================

    Mixer.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
    * Class used to mix dry sound with wet sound after convolution.
    */
    class Mixer : public Task
    {
    public:
        //==============================================================================
        Mixer(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<Mixer>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& wetAudio) override;

        //==============================================================================
        void loadDry(const juce::AudioSampleBuffer& dryAudio);

        //==============================================================================
        double wetRatio=0.0;
		juce::AudioSampleBuffer& dryAudio;

    };

}
