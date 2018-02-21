/*
  ==============================================================================

    Filter.cpp

  ==============================================================================
*/

#include "Filter.h"
#include "PluginParameters.h"

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
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool Filter::updateParams(const std::string& blockId)
    {
        auto& params = getMapOfParams();
        bool changedParams = false;

        // Frequency
        auto paramFreq = dynamic_cast<juce::AudioParameterFloat*>(params.at(blockId + "_freq"));

        if (!paramFreq)
        {
            throw std::invalid_argument("Received non-float parameter for frequency in Filter block");
        }

        if (*paramFreq != frequency)
        {
            frequency = *paramFreq;
            changedParams = true;
        }

        // Q factor
        auto paramQ = dynamic_cast<juce::AudioParameterFloat*>(params.at(blockId + "_Q"));

        if (!paramQ)
        {
            throw std::invalid_argument("Received non-float parameter for Q factor in Filter block");
        }

        if (*paramQ != Q)
        {
            Q = *paramQ;
            changedParams = true;
        }

        // Gain
        auto paramGain = dynamic_cast<juce::AudioParameterFloat*>(params.at(blockId + "_gain"));

        if (!paramGain)
        {
            throw std::invalid_argument("Received non-float parameter for Q factor in Filter block");
        }

        if (*paramGain != gainFactor)
        {
            gainFactor = *paramGain;
            changedParams = true;
        }

        return changedParams;
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
