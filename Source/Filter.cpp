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
		: Task(processor), frequency(freq), Q(q), gainFactor(gain)
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
			throw WrongParameter();


		buildFilter();
		juce::dsp::AudioBlock<float> block(ir);

		juce::dsp::ProcessContextReplacing<float> context(block);
		juce::dsp::IIR::Filter<float>::process(context);
		
    }

	void Filter::setFrequency(float freq) {

		if (freq < 0 || freq > 20000)
		    throw WrongParameter();

		frequency = freq;
	}

	void Filter::setQ(float q) {

		if(q < 0.7)
		    throw WrongParameter();

		Q = q;
	}

	void Filter::setGain(float gain) {

		if(gain < 0 || gain > invdB(15))
		    throw WrongParameter();

		gainFactor = gain;
	}

	bool Filter::assertValues() {
		if (frequency > 0 && frequency < 20000 && Q > 0.7 && gainFactor >= 0 && gainFactor < invdB(15))
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
			throw WrongParameter();

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
			throw WrongParameter();

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
			throw WrongParameter();

		coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), frequency, Q, gainFactor);
    }

}
