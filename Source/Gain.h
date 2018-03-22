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

        //==============================================================================
        using Ptr = std::shared_ptr<Gain>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) override;

        virtual AudioBlock exec(AudioBlock buffer) override;

    protected:
        //==============================================================================
        float gainFactor = 1.0f;
    };

}