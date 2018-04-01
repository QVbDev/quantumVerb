/*
  ==============================================================================

    IRPipeline.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include "Equalizer.h"
#include "Gain.h"
#include "IRBank.h"
#include "PreDelay.h"
#include "TimeStretch.h"

#include <array>
#include <mutex>
#include <string>

namespace reverb
{

    //==============================================================================
    /**
     * A pipeline implementing various transformation steps that are applied to impulse responses.
     * This pipeline is executed only as needed (i.e. when a new IR is requested or when one of
     * its parameters is changed). Therefore, it is not part of the critical path (its speed
     * does not have a huge impact on plugin performance).
     */
    class IRPipeline : public Task
    {
    public:
        //==============================================================================
        IRPipeline(juce::AudioProcessor * processor, int channelIdx);

        //==============================================================================
        using Ptr = std::shared_ptr<IRPipeline>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& = "") override;

        virtual AudioBlock exec(AudioBlock = AudioBlock()) override;

        //==============================================================================
        virtual bool needsToRun() const override;

        //==============================================================================
        virtual void updateSampleRate(double sr) override;

        //==============================================================================
        AudioBlock reloadIR();

        static constexpr float MAX_IR_INTENSITY = 0.5f;

    protected:
        //==============================================================================
        Equalizer::Ptr equalizer;
        TimeStretch::Ptr timeStretch;
        Gain::Ptr gain;
        PreDelay::Ptr preDelay;

        //==============================================================================
        double lastSampleRate = 0;

        //==============================================================================
        int channelIdx;

        //==============================================================================
        std::string irNameOrFilePath = "";

        juce::AudioSampleBuffer ir;

        void loadIRFromBank(const std::string& irBuffer);
        void loadIRFromDisk(const std::string& irFilePath);
    };

}
