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
		buildFilter();
		juce::dsp::AudioBlock<float> block(ir);

		juce::dsp::ProcessContextReplacing<float> context(block);
		juce::dsp::IIR::Filter<float>::process(context);
		
    }

	void Filter::setFrequency(float freq) {
		jassert(freq > 0 && freq <= 20000);
		frequency = freq;
	}

	void Filter::setQ(float q) {
		jassert(q > 0.7);
		Q = q;
	}

	void Filter::setGain(float gain) {
		jassert(gain >= 0 && gain < invdB(15));
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
		jassert(assertValues());
		coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(processor->getSampleRate(), LowShelfFilter::frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void HighShelfFilter::buildFilter()
    {
		jassert(assertValues());
		coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(processor->getSampleRate(), HighShelfFilter::frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void PeakFilter::buildFilter()
    {
		jassert(assertValues());
		coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), PeakFilter::frequency, Q, gainFactor);
    }

}
