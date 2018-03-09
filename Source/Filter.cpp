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
     * @brief Constructs a Filter object with optional frequency/gain/Q parameters
     *
     * The Filter object is constructed from the processor pointer and from the optional frequency/gain/Q parameters.
     * The frequency is the cut-off frequency of the derived LowShelfFilter/HighShelfFilter and the center frequency of the PeakFilter class.
     *
     * @param [in] processor    Pointer to main processor
     * @param [in] freq    Band frequency
     * @param [in] gain    Band gain
     * @param [in] q    Q factor
     */
    Filter::Filter(juce::AudioProcessor * processor, float freq, float q, float gain)
        : Task(processor), isOn(true), frequency(freq), Q(q), gainFactor(gain)
    {
        
    }
    
    //==============================================================================
    /**
     * @brief Read processor parameters and update block parameters as necessary
     *
     * @returns True if any parameters were changed, false otherwise.
     * @throws ChannelNumberException
     * @throws WrongParameterException
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

    //==============================================================================
    /**
    * @brief Gives the filter absolute amplitude response at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    */

    float Filter::getAmplitude(float freq){
        // All filters are 2nd order and thus have five coefficients
        float * coeffs = coefficients->getRawCoefficients();

        float b0 = coeffs[0];
        float b1 = coeffs[1];
        float b2 = coeffs[2];
        float a1 = coeffs[3];
        float a2 = coeffs[4];

        //Compute transfer function argument e^(jw)
        std::complex<float> input;
        std::complex<float> output;

        float minus_omega = -(2 * M_PI * freq) / processor->getSampleRate();

        input.real(std::cos(minus_omega));
        input.imag(std::sin(minus_omega));

        //Compute and return transfer function amplitude

        return std::abs(b0 + b1 * input + b2 * std::pow(input, 2)) / std::abs(1.0f + a1 * input + a2 * std::pow(input, 2));
    }

    //==============================================================================
    /**
    * @brief Gives the filter amplitude response in dB at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    *
    * @return Filter amplitude response in dB
    */

    float Filter::getdBAmplitude(float freq) 
    {
        return 20 * std::log10(getAmplitude(freq));
    }

    //==============================================================================
    /**
    * @brief Sets the filter frequency and updates the IIR filter coefficients
    *
    * @param [in] freq   Frequency to be set
    */
    

    void Filter::setFrequency(float freq) 
    {

        if (freq <= 0 || freq > FMAX) throw WrongParameterException();
        
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
    */

        return mustExec;
    }

        if(q < QMIN || q > QMAX) throw WrongParameterException();

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
    //==============================================================================
    /**
    * @brief Sets the filter band gain and updates the IIR filter coefficients
    *
    * @param [in] freq   Band gain to be set
    */
		
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

    bool Filter::isEnabled() 
    {
        return isOn;
    }

    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */

    void Filter::enable() 
    {
        isOn = true;
    }

    /**
    * @brief (TODO) Brief description
    *
    * (TODO) Detailed description
    */

    void Filter::disable() 
    {
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
