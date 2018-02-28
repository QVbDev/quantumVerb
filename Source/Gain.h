/*
  ==============================================================================

    Gain.h

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
    class Gain : public Task
    {
    public:
        //==============================================================================
        Gain(juce::AudioProcessor * processor);
        ~Gain() = default;

        //==============================================================================
        using Ptr = std::shared_ptr<Gain>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) override;

        virtual void exec(juce::AudioSampleBuffer& buffer) override;

    protected:
        //==============================================================================
        float gainFactor = 1.0f;
    };

}