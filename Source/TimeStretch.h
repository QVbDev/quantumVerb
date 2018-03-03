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
                                  const juce::String& blockID) override;

        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        void setOrigIRSampleRate(double sampleRate);

    protected:
        //==============================================================================
        static constexpr double MAX_IR_LENGTH_S = 5.0f;

        float irLengthS = 3.0f;

        //==============================================================================
        // TODO: Do we need to keep this information? Or is TimeStretch linear (i.e.
        //       same result if sample rate differential is constant)?
        double origIRSampleRate;

        //==============================================================================
        std::unique_ptr<soundtouch::SoundTouch> soundtouch;
    };

}
