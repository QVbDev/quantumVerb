/*
  ==============================================================================

    IRPipeline.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include "Filter.h"
#include "Gain.h"
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
        IRPipeline(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<IRPipeline>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        void loadIR(const std::string& irFilePath);

        //==============================================================================
        std::array<Filter::Ptr, 4> filters;
        TimeStretch::Ptr timeStretch;
        Gain::Ptr gain;
        PreDelay::Ptr preDelay;

        //==============================================================================
        static constexpr int MAX_IR_LENGTH_S = 5;

        bool mustExec = true;

    protected:
        std::vector<juce::AudioSampleBuffer> irChannels;
    };

}
