/*
  ==============================================================================

    Filter.cpp

  ==============================================================================
*/

#include "Filter.h"
#include "PluginProcessor.h"

#include "Logger.h"

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
        // Frequency
        auto paramFreq = params.getRawParameterValue(blockId + AudioProcessor::PID_FILTER_FREQ_SUFFIX);

        if (!paramFreq)
        {
            throw std::invalid_argument("Received non-float parameter for frequency in Filter block");
        }

        if (*paramFreq != frequency)
        {
            frequency = *paramFreq;
            mustExec = true;
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
            mustExec = true;
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
            mustExec = true;
        }

        // Rebuild filter if necessary
        if (mustExec)
        {
            buildFilter();
        }

        return mustExec;
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
        if (mustExec)
        {
            buildFilter();

            if (ir.getNumChannels() != 1)
            {
                logger.dualPrint(Logger::Level::Warning, "Filter: AudioBuffer channel count is not 1");
                return;
            }

            if (isOn)
            {
                juce::dsp::AudioBlock<float> block(ir);

                juce::dsp::ProcessContextReplacing<float> context(block);
                juce::dsp::IIR::Filter<float>::process(context);

                // Reset mustExec flag
                mustExec = false;
            }
        }
    }

    //==============================================================================
    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
	bool Filter::isEnabled() {
		return isOn;
	}

    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
	void Filter::enable() {
		isOn = true;
	}

    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
	void Filter::disable() {
		isOn = false;

	}

    //==============================================================================
    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
    void LowShelfFilter::buildFilter()
    {
		coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(processor->getSampleRate(), frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
    void HighShelfFilter::buildFilter()
    {
		coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(processor->getSampleRate(), frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */
    void PeakFilter::buildFilter()
    {
		coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), frequency, Q, gainFactor);
    }

}
