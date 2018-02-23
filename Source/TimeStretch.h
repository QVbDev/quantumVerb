/*
  ==============================================================================

    TimeStretch.h

  ==============================================================================
*/

#pragma once

#include "SoundTouch.h"

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
     * Implements a time stretching algorithm to manage IR buffer length and sample rate.
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
        std::unique_ptr<soundtouch::SoundTouch> soundtouch;
    };

}
