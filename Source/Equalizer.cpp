/*
==============================================================================

Equalizer.cpp

==============================================================================
*/

#include "Equalizer.h"

namespace reverb {

	Equalizer::Equalizer(juce::AudioProcessor * processor, int numFilters)
		: Task(processor) 
	{

		if (numFilters < 3) throw WrongFilterNumber();

		filterSet.add(new LowShelfFilter(processor));
		
		for (int i = 0; i < numFilters - 2; i++) 
		{
			filterSet.add(new PeakFilter(processor));
		}

		filterSet.add(new HighShelfFilter(processor));

		for (int i = 0; i < numFilters; i++) 
		{
			EQGains.push_back(1.0f);
		}

		//These constructor setters will likely need to be updated in the very near future.

		filterSet[0]->setQ(0.71);
		filterSet[numFilters - 1]->setQ(0.71);

		for (int i = 0; i < numFilters; i++)
		{
			EQGains[i] = 2;
			filterSet[i]->setGain(2);
			filterSet[i]->setFrequency((i + 1) * 10000 / numFilters);

			if (i != 0 && i != numFilters - 1) filterSet[i]->setQ(4);
		}

		

		
	}

	void Equalizer::exec(juce::AudioSampleBuffer& ir) 
	{
		updateFilters();

		for (int i = 0; i < filterSet.size(); i++) 
		{
			filterSet[i]->exec(ir);
		}
	}

	void Equalizer::updateFilters() 
	{

		//Update filter parameters before gain normalization

		const int dim = filterSet.size();

		std::vector<float> evalFrequencies(dim);

		//Set evaluation frequencies

		evalFrequencies[0] = 0;

		for (int i = 1; i < dim - 1; i++) 
		{
			evalFrequencies[i] = filterSet[i]->frequency;
		}

		evalFrequencies[(dim - 1)] = FMAX;

		//Create Matrix objects

		juce::dsp::Matrix<float> B(dim, dim);
		juce::dsp::Matrix<float> gamma(dim, 1);
		juce::dsp::Matrix<float> lambda(dim, 1);

		float * B_data = B.getRawDataPointer();
		float * gamma_data = gamma.getRawDataPointer();
		float * lambda_data = lambda.getRawDataPointer();

		//Compute gamma column

		for (int i = 0; i < dim; i++) 
		{
			gamma_data[i] = filterSet[i]->getdBAmplitude(evalFrequencies[i]);
		}

		//Set gains to 1 dB for B matrix
		for (int i = 0; i < dim; i++) 
		{
			filterSet[i]->setGain(Filter::invdB(1.0f));
		}
		


		//Correction algorithm (5 iterations)

		for (int k = 0; k < 5; k++) 
		{

			memcpy(lambda_data, gamma_data, dim * sizeof(float));

			//Compute B matrix with new gains
			for (int i = 0; i < dim; i++) 
			{

				for (int j = 0; j < dim; j++) 
				{

					B_data[j + B.getNumColumns() * i] = filterSet[j]->getdBAmplitude(evalFrequencies[i]);
				}
			}

			B.solve(lambda);

			for (int i = 0; i < dim; i++) 
			{

				filterSet[i]->setGain(Filter::invdB(lambda_data[i] * Filter::todB(filterSet[i]->gainFactor)));
			}
		}


	}

	/**
	* @brief Gives the Equalizer amplitude response in dB at a given frequency
	*
	* @param [in] freq   Frequency at which the filter magnitude is evaluated
	*/

	float Equalizer::getdBAmplitude(float freq) 
	{
		float dBAmplitude = 0;

		for (int i = 0; i < filterSet.size(); i++) 
		{
			dBAmplitude += filterSet[i]->getdBAmplitude(freq);
		}

		return dBAmplitude;

	}

	void Equalizer::setFilterFrequency(float freq, int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

		//Check whether filter frequency crosses another one's

		if (num == 0) 
		{
			if (freq >= filterSet[num + 1]->frequency) throw WrongEQFrequency();
		}

		if (num == (filterSet.size() - 1))
		{
			if (freq <= filterSet[num - 1]->frequency) throw WrongEQFrequency();
		}

		else 
		{
			if (freq <= filterSet[num - 1]->frequency || freq >= filterSet[num + 1]->frequency) throw WrongEQFrequency();
		}

			filterSet[num]->setFrequency(freq);
	}
	void Equalizer::setFilterGain(float gain, int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

			EQGains[num] = gain;

			filterSet[num]->setGain(gain);

	}
	void Equalizer::setFilterQ(float Q, int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

			filterSet[num]->setQ(Q);
	}

	float Equalizer::getFilterFrequency(int num) 
	{

		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

		return filterSet[num]->frequency;
	}

	float Equalizer::getFilterGain(int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

		return filterSet[num]->gainFactor;
	}

	float Equalizer::getFilterQ(int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

		return filterSet[num]->gainFactor;
	}

	float Equalizer::getEQGain(int num) 
	{
		if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

		return EQGains[num];
	}

	int Equalizer::getNumFilters() 
	{
		return filterSet.size();
	}
}