/*
==============================================================================
Filter.h
==============================================================================
*/

#pragma once

#include "Task.h"

#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace reverb
{

    //==============================================================================
    /**
    * The Filter class is the abstract class from which the three filter types of the plugin are derived.
    */
    class Filter : public Task,
        protected juce::dsp::IIR::Filter<float>
    {

        friend class Equalizer;
        friend class EqualizerMocked;

    public:
        //==============================================================================
        Filter(juce::AudioProcessor * processor, float freq = 1000.0f, float q = 0.71f, float gain = 1.5f);

        //==============================================================================
        using Ptr = std::shared_ptr<Filter>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
            const juce::String& blockId) override;

        virtual AudioBlock exec(AudioBlock ir) override;

        //==============================================================================
        static float todB(float m)
        {
            return 20 * std::log10(m);
        }

        //==============================================================================
        static float invdB(float dB)
        {
            return pow(10, dB / 20);
        }

        //==============================================================================

        float getAmplitude(float freq);
        float getdBAmplitude(float freq);

        bool isEnabled();
        void enable();
        void disable();

    protected:
        void setFrequency(float);
        void setQ(float);
        void setGain(float);

        virtual void buildFilter() = 0;

        bool isOn;

        float frequency;
        float Q;
        float gainFactor;

    };


    //==============================================================================
    /**
    * This LowShelfFilter class implements a low-shelf IIR filter
    * The inherited exec() function is used to process samples.
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
    * This HighShelfFilter class implements a high-shelf IIR filter
    * The inherited exec() function is used to process samples.
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
    * This PeakFilter class implements a peaking IIR filter
    * The inherited exec() function is used to process samples.
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