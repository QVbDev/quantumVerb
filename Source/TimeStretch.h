/*
  ==============================================================================

    TimeStretch.h

  ==============================================================================
*/

#pragma once

#include "SoundTouchDLL.h"

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
    * TODO: Description
    */
    class TimeStretch : public Task
    {
    public:
        //==============================================================================
        TimeStretch(juce::AudioProcessor * processor);
        ~TimeStretch() noexcept;

        //==============================================================================
        using Ptr = std::shared_ptr<TimeStretch>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        double origIRSampleRate;

    private:
        HANDLE soundtouchHandle = nullptr;
    };

}
