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

	//==============================================================================
	/**
	* @brief Gives the filter absolute amplitude response at a given frequency
	*
	* @param [in] freq   Frequency at which the filter magnitude is evaluated
	*/

	float Filter::getAmplitude(float freq){
		// All filters are 2nd order
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
	*/

	float Filter::getdBAmplitude(float freq) {
		return 20 * std::log10(getAmplitude(freq));
	}

	//==============================================================================
	/**
	* @brief Sets the filter frequency and updates the IIR filter coefficients
	*
	* @param [in] freq   Frequency to be set
	*/
	

	void Filter::setFrequency(float freq) {

		if (freq <= 0 || freq > FMAX)
		    throw WrongParameterException();

		frequency = freq;

		buildFilter();
	}

	//==============================================================================
	/**
	* @brief Sets the filter Q factor and updates the IIR filter coefficients
	*
	* @param [in] freq   Q factor to be set
	*/

	void Filter::setQ(float q) {

		if(q < QMIN || q > QMAX)
		    throw WrongParameterException();

		Q = q;

		buildFilter();
	}

	//==============================================================================
	/**
	* @brief Sets the filter band gain and updates the IIR filter coefficients
	*
	* @param [in] freq   Band gain to be set
	*/

	void Filter::setGain(float gain) {

		if(gain < invdB(GMIN) || gain > invdB(GMAX))
		    throw WrongParameterException();

		gainFactor = gain;

		buildFilter();
	}

	//==============================================================================
	/**
	* @brief Indicates whether the filter is enabled or not
	*
	* @param [out] bool   True if filter is enabled or false if filter is disabled
	*/

	bool Filter::isEnabled() {
		return isOn;
	}

	//==============================================================================
	/**
	* @brief Enables the filter
	*
	*/

	void Filter::enable() {
		isOn = true;
	}

	//==============================================================================
	/**
	* @brief Disables the filter
	*
	*/

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
