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
    * TODO: Description
    */
    class Mixer : public Task
    {
    public:
        //==============================================================================
        Mixer(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<Mixer>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) override;

        virtual void exec(juce::AudioSampleBuffer& wetAudio) override;

        //==============================================================================
        void loadDry(const juce::AudioSampleBuffer& dryAudio);

    protected:
        //==============================================================================
        float wetRatio;
    };

}
