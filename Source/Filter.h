/*
  ==============================================================================

    Filter.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include <memory>

namespace reverb
{

    //==============================================================================
    /**
    * TODO: Description
    */
    class Filter : public Task,
                   public juce::dsp::IIR::Filter<float>
    {
    public:
        //==============================================================================
        Filter(juce::AudioProcessor * processor);

        //==============================================================================
        using Ptr = std::shared_ptr<Filter>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
        virtual void buildFilter() = 0;

        //==============================================================================
        double freq;
        double Q;
        double gainFactor;
    };

    
    //==============================================================================
    /**
    * TODO: Description
    */
    class LowShelfFilter : public Filter
    {
    public:
        //==============================================================================
        using Filter::Filter;   // Inherit constructor
        
        //==============================================================================
        virtual void buildFilter() override;
    };

    
    //==============================================================================
    /**
    * TODO: Description
    */
    class HighShelfFilter : public Filter
    {
    public:
        //==============================================================================
        using Filter::Filter;   // Inherit constructor
        
        //==============================================================================
        virtual void buildFilter() override;
    };


    //==============================================================================
    /**
    * TODO: Description
    */
    class PeakFilter : public Filter
    {
    public:
        //==============================================================================
        using Filter::Filter;   // Inherit constructor
        
        //==============================================================================
        virtual void buildFilter() override;
    };

}