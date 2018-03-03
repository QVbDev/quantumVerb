/*
  ==============================================================================

    PreDelay.h
    Created: 14 Feb 2018 3:26:12pm
    Author:  Eric Seguin

  ==============================================================================
*/

#pragma once

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
    * Implements a method to apply a pre-delay factor to a given impulse response.
    */
    class PreDelay : public Task
    {
    public:
        //==============================================================================
        PreDelay(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<PreDelay>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) override;

        virtual void exec(juce::AudioSampleBuffer& ir) override;

    protected:
        //==============================================================================
        static constexpr int MAX_DELAY_MS = 1000;

        //==============================================================================
        float delayMs = 0;
    };

}
