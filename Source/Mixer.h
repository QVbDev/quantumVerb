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
        ~Mixer() = default;

        //==============================================================================
        using Ptr = std::shared_ptr<Mixer>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) override;

        virtual void exec(juce::AudioSampleBuffer& wetAudio) override;

        //==============================================================================
        void loadDry(const juce::AudioSampleBuffer dryAudio);

    protected:
        //==============================================================================
        juce::AudioSampleBuffer dryAudio;


        //==============================================================================
        float wetRatio = 0.0f;
    };

}
