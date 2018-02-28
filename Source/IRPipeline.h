/*
  ==============================================================================

    IRPipeline.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include "Filter.h"
#include "Gain.h"
#include "IRBank.h"
#include "PreDelay.h"
#include "TimeStretch.h"

#include <array>
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
        IRPipeline(juce::AudioProcessor * processor, const IRBank& irBank, int channelIdx);

        //==============================================================================
        using Ptr = std::shared_ptr<IRPipeline>;

        //==============================================================================
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& = "") override;

        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        virtual bool needsToRun() const override;

        //==============================================================================
        bool updateSampleRate(double sampleRate);

        //==============================================================================
        void loadIR(const std::string& irNameOrFilePath);

    protected:
        //==============================================================================
        std::array<Filter::Ptr, 4> filters;
        TimeStretch::Ptr timeStretch;
        Gain::Ptr gain;
        PreDelay::Ptr preDelay;

        //==============================================================================
        static constexpr int MAX_IR_LENGTH_S = 5;

        //==============================================================================
        double lastSampleRate = 0;

        //==============================================================================
        void loadIRFromBank(const std::string& irBuffer);
        void loadIRFromDisk(const std::string& irFilePath);

        int channelIdx;
        juce::AudioSampleBuffer irChannel;

        //==============================================================================
        const IRBank& irBank;
        juce::String currentIR = "";
    };

}
