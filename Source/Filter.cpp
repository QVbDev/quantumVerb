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
     * @param [in] params   the value tree containing the parameters
     * @param [in] blockId  the Id of the relevant parameter block
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

        return mustExec;
    
    }

    //==============================================================================
    /**
    * @brief Filters the audio in AudioSampleBuffer
    *
    * This function filters an AudioBuffer using the IIR filter's coefficients
    *
    * @param [in,out] ir   Contains the audio to be filtered, the output is placed in that same buffer
    * @throws ChannelNumberException
    * @throws WrongParameterException
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
    * @brief Returns the filter absolute amplitude response at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    */


    float Filter::getAmplitude(float freq) {
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

        float minusOmega = -(2 * M_PI * freq) / processor->getSampleRate();

        input.real(std::cos(minusOmega));
        input.imag(std::sin(minusOmega));

        //Compute and return transfer function amplitude

        return std::abs(b0 + b1 * input + b2 * std::pow(input, 2)) / std::abs(1.0f + a1 * input + a2 * std::pow(input, 2));
    }

    //==============================================================================
    /**
    * @brief Returns the filter amplitude response in dB at a given frequency
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
    * @brief Sets the filter frequency and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Frequency to be set
    */


    void Filter::setFrequency(float freq)
    {

        frequency = freq;

        buildFilter();
    }

    //==============================================================================
    /**
    * @brief Sets the filter Q factor and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Q factor to be set
    */

    void Filter::setQ(float q)
    {

        Q = q;

        buildFilter();
    }
    //==============================================================================
    /**
    * @brief Sets the filter band gain and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Band gain to be set
    */

    void Filter::setGain(float gain)
    {

        gainFactor = gain;

        buildFilter();
    }

    //==============================================================================
    /**
    * @brief Indicates whether the filter is enabled or not
    *
    * @return   True if filter is enabled or false if filter is disabled
    */

    bool Filter::isEnabled()
    {
        return isOn;
    }

    //==============================================================================
    /**
    * @brief Enables the filter
    *
    */

    void Filter::enable()
    {
        isOn = true;
    }

    //==============================================================================
    /**
    * @brief Disables the filter
    *
    */

    void Filter::disable()
    {
        isOn = false;

    }

    //==============================================================================
    /**
    * @brief Generates and sets the low-shelf IIR filter coefficients according to the current filter parameters
    *
    */
    void LowShelfFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Generates and sets the high-shelf IIR filter coefficients according to the current filter parameters
    *
    */
    void HighShelfFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Generates and sets the peak IIR filter coefficients according to the current filter parameters
    *
    */
    void PeakFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

}
