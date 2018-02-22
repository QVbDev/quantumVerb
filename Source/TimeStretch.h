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
        ~TimeStretch() = default;

        //==============================================================================
        using Ptr = std::shared_ptr<TimeStretch>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& = "") override;

        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        double origIRSampleRate;

    private:
        //==============================================================================
        std::unique_ptr<soundtouch::SoundTouch> soundtouch;
    };

}
