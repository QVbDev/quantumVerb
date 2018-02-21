/*
  ==============================================================================

    Filter.cpp

  ==============================================================================
*/

#include "Filter.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in] processor    Pointer to main processor
     */
	Filter::Filter(juce::AudioProcessor * processor, float freq, float q, float gain)
		: Task(processor),isOn(true), frequency(freq), Q(q), gainFactor(gain)
    {
    }

    //==============================================================================
    /**
     * @brief Brief description
     *
     * Detailed description
     *
     * @param [in,out] ir   Parameter description
     */
    void Filter::exec(juce::AudioSampleBuffer& ir)
    {
		if (ir.getNumChannels() != 1)
			throw ChannelNumberException();

		if (!assertValues())
			throw WrongParameterException();
		
		if (isOn) {

			buildFilter();
			juce::dsp::AudioBlock<float> block(ir);

			juce::dsp::ProcessContextReplacing<float> context(block);
			juce::dsp::IIR::Filter<float>::process(context);
		}
		
    }

	void Filter::setFrequency(float freq) {

		if (freq <= 0 || freq > FMAX)
		    throw WrongParameterException();

		frequency = freq;
	}

	void Filter::setQ(float q) {

		if(q < QMIN || q > QMAX)
		    throw WrongParameterException();

		Q = q;
	}

	void Filter::setGain(float gain) {

		if(gain < invdB(GMIN) || gain > invdB(GMAX))
		    throw WrongParameterException();

		gainFactor = gain;
	}

	bool Filter::isEnabled() {
		return isOn;
	}

	void Filter::enable() {
		isOn = true;
	}

	void Filter::disable() {
		isOn = false;

	}

	bool Filter::assertValues() {
		if (frequency > 0 && frequency <= FMAX && Q >= QMIN && Q <= QMAX && gainFactor >= invdB(GMIN) && gainFactor <= invdB(GMAX))
			return true;
		else
			return false;
	}

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void LowShelfFilter::buildFilter()
    {
		if (!assertValues())
			throw WrongParameterException();

		coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(processor->getSampleRate(), frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void HighShelfFilter::buildFilter()
    {
		if (!assertValues())
			throw WrongParameterException();

		coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(processor->getSampleRate(), frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void PeakFilter::buildFilter()
    {
		if (!assertValues())
			throw WrongParameterException();

		coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), frequency, Q, gainFactor);
    }

}
