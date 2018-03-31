/*
==============================================================================

Equalizer.h

==============================================================================
*/

#pragma once

#include "Task.h"
#include "Filter.h"

#include <memory>
#include <exception>
#include <string>


namespace reverb {

    //==============================================================================
    /**
    * The Equalizer class cimplements by default four Filter class instances -one low-shelf, two peak filters and one high-shelf - 
    * and calibrates their individual gains to balance out the stacking effect thus making sure that the EQ band gains are equal to the ones set by the user. 
    *
    *It acts as an interface between the GUI and the IIR filters.
    *The class can support one low-shelf filter and one high-shelf with any number of peak filters in between; the total number is equal to the numFilters constructor argument. 
    *The filter instantiations are done in the class constructor.
    */
    class Equalizer : public Task 
    {

    public:

        Equalizer(juce::AudioProcessor * processor, int numFilters = 4);
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
            const juce::String& blockId) override;

        using Ptr = std::shared_ptr<Equalizer>;


        virtual AudioBlock exec(AudioBlock ir) override;

        virtual void updateSampleRate(double sr) override;


        void calibrateFilters();

        float getdBAmplitude(float freq);

        int getNumFilters();
        virtual bool needsToRun() const override;


    protected:
        juce::OwnedArray<Filter> filterSet;
        std::vector<float> EQGains;
    };

    struct InvalidFilterException : public std::exception 
    {
        const char * what() const throw () 
        {
            return "Equalizer: Filter ID is invalid";
        }
    };

    struct ConvergenceException : public std::exception
    {
        const char * what() const throw ()
        {
            return "Equalizer: The calibration algorithm did not converge";
        }
    };
}