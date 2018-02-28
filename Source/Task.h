/*
  ==============================================================================

    Task.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace reverb
{

    //==============================================================================
    /**
     * TODO: Description
     */
    class Task
    {
    public:
        //==============================================================================
        Task(juce::AudioProcessor * processor) : processor(processor) {}

        //==============================================================================
        using Ptr = std::shared_ptr<Task>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) = 0;

        virtual void exec(juce::AudioSampleBuffer&) = 0;

        virtual bool needsToRun() const { return mustExec; }

    protected:
        //==============================================================================
        juce::AudioProcessor * processor;

        //==============================================================================
        bool mustExec = true;
    };

}
