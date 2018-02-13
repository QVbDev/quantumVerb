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
    Filter::Filter(juce::AudioProcessor * processor)
        : Task(processor)
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

    //==============================================================================
    /**
    * @brief Brief description
    *
    * Detailed description
    */
    void LowShelfFilter::buildFilter()
    {
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
		jassert(frequency > 0);
		jassert(frequency > 0);
		jassert(gainFactor > 0 && gainFactor < invdB(15));
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
		coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), frequency, Q, gainFactor);
    }

}
