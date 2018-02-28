/*
  ==============================================================================

    Filter.h

  ==============================================================================
*/

#pragma once

#include "Task.h"

#include <memory>
#include <exception>

# define M_PI 3.14159265358979323846f

#define QMIN 0.2
#define QMAX 6.5
#define GMIN -24
#define GMAX 15
#define FMAX 21000

namespace reverb
{

    //==============================================================================
    /**
    * TODO: Description
    */
    class Filter : public Task,
                   protected juce::dsp::IIR::Filter<float>
    {

	friend class Equalizer;

    public:
        //==============================================================================
        Filter(juce::AudioProcessor * processor, float freq = 1000.0f, float q = 0.71f, float gain = 1.5f);

        //==============================================================================
        using Ptr = std::shared_ptr<Filter>;

        //==============================================================================
        virtual void exec(juce::AudioSampleBuffer& ir) override;

		//==============================================================================
		static float todB(float m) {
			return 20 * std::log10(m);
		}

        //==============================================================================
        static float invdB(float dB) {
			return pow(10, dB / 20);
        }

        //==============================================================================

		float getAmplitude(float freq);
		float getdBAmplitude(float freq);

		void setFrequency(float);
		void setQ(float);
		void setGain(float);

		bool isEnabled();
		void enable();
		void disable();

	protected:
		bool assertValues();
		virtual void buildFilter() = 0;
		
		bool isOn;

		float frequency;
		float Q;
		float gainFactor;

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

	struct WrongParameterException : public std::exception {
		const char * what() const throw () {
			return "Filter: Parameter(s) is out of bounds";
		}
	};

}