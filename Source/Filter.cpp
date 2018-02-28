/*
  ==============================================================================

    Filter.cpp

  ==============================================================================
*/

#include "Filter.h"
#include "PluginProcessor.h"

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
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool Filter::updateParams(const juce::AudioProcessorValueTreeState& params,
                              const juce::String& blockId)
    {
        bool changedParams = false;

        // Frequency
        auto paramFreq = params.getRawParameterValue(blockId + AudioProcessor::PID_FILTER_FREQ_SUFFIX);

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
        auto paramQ = params.getRawParameterValue(blockId + AudioProcessor::PID_FILTER_Q_SUFFIX);

        if (!paramQ)
        {
            throw std::invalid_argument("Parameter not found for Q factor in Filter block");
        }

        if (*paramQ != Q)
        {
            Q = *paramQ;
            changedParams = true;
        }

        // Gain
        auto paramGain = params.getRawParameterValue(blockId + AudioProcessor::PID_FILTER_GAIN_SUFFIX);

        if (!paramGain)
        {
            throw std::invalid_argument("Parameter not found for Q factor in Filter block");
        }
        
        if (*paramGain != gainFactor)
        {
            gainFactor = *paramGain;
            changedParams = true;
        }

        // Rebuild filter if necessary
        if (changedParams)
        {
            buildFilter();
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
			throw WrongParameterException();
		
		if (isOn) {

			buildFilter();
			juce::dsp::AudioBlock<float> block(ir);

			juce::dsp::ProcessContextReplacing<float> context(block);
			juce::dsp::IIR::Filter<float>::process(context);
		}
		
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
