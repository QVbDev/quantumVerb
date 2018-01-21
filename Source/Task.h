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
        virtual void exec(juce::AudioSampleBuffer&) = 0;

    protected:
        juce::AudioProcessor * processor;
    };

}
