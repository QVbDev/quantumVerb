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

        virtual AudioBlock exec(AudioBlock wetAudio) override;

        //==============================================================================
        void loadDry(AudioBlock audio);

    protected:
        //==============================================================================
        juce::AudioSampleBuffer dryAudioCopy;


        //==============================================================================
        float wetRatio = 0.0f;
    };

}
