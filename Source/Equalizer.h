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
        virtual bool updateParams(const juce::AudioProcessorValueTreeState& params,
            const juce::String& blockId) override;

        using Ptr = std::shared_ptr<Equalizer>;


        virtual void exec(juce::AudioSampleBuffer& ir) override;

        void calibrateFilters();

        float getdBAmplitude(float freq);

        void setFilterFrequency(float freq, int num);
        void setFilterGain(float gain, int num);
        void setFilterQ(float Q, int num);

        float getFilterFrequency(int num);
        float getFilterGain(int num);
        float getFilterQ(int num);

        void enableFilter(int num);
        void disableFilter(int num);

        float getEQGain(int num);

        int getNumFilters();

        virtual bool needsToRun() const override;


    private:
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

    struct WrongEQFrequency : public std::exception 
    {
        const char * what() const throw () 
        {
            return "Equalizer: Filter frequency crosses over another one's ";
        }
    };

    struct WrongFilterNumber : public std::exception 
    {
        const char * what() const throw () {
            return "Equalizer: Filter number must be at least 3 ";
        }
    };
}