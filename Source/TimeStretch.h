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
    * TimeStretch class: implements a time stretching algorithm to manage IR buffer length
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
