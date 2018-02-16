/*
  ==============================================================================

    Filter.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include <memory>
#include <exception>

namespace reverb
{

	static float invdB(float dB) {
		return pow(10, dB / 10);
	}

    //==============================================================================
    /**
    * TODO: Description
    */
    class Filter : public Task,
                   protected juce::dsp::IIR::Filter<float>
    {
    public:
        //==============================================================================
        Filter(juce::AudioProcessor * processor, float freq = 1000.0f, float q = 0.71f, float gain = 1.0f);

        //==============================================================================
        using Ptr = std::shared_ptr<Filter>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& ir) override;

        //==============================================================================
	

        //==============================================================================

		void setFrequency(float);
		void setQ(float);
		void setGain(float);

	protected:
		bool assertValues();
		virtual void buildFilter() = 0;
		

		double frequency;
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

	//==============================================================================
	/**
	* Exceptions for Filter class
	*/
	struct ChannelNumberException : public std::exception {
		const char * what() const throw () {
			return "Filter: AudioBuffer channel number is not 1";
		}
	};

	struct WrongParameter : public std::exception {
		const char * what() const throw () {
			return "Filter: Parameter(s) is out of bounds";
		}
	};

}