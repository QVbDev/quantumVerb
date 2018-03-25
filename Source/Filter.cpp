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

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void Filter::updateParams(const juce::AudioProcessorValueTreeState& params,
                              const juce::String& blockId)
    {
        // Frequency
        float _frequency = getParam(params,
                                    blockId + AudioProcessor::PID_FILTER_FREQ_SUFFIX);

        if (frequency != _frequency)
        {
            frequency = _frequency;
            mustExec = true;
        }

        // Q factor
        float _Q = getParam(params,
                            blockId + AudioProcessor::PID_FILTER_Q_SUFFIX);

        if (Q != _Q)
        {
            Q = _Q;
            mustExec = true;
        }

        // Gain factor
        float _gainFactor = getParam(params,
                                     blockId + AudioProcessor::PID_FILTER_GAIN_SUFFIX);

        if (gainFactor != _gainFactor)
        {
            gainFactor = _gainFactor;
            mustExec = true;
        }
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
