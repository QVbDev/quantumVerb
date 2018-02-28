/*
==============================================================================

Equalizer.cpp

==============================================================================
*/

#include "Equalizer.h"

namespace reverb {

	Equalizer::Equalizer(juce::AudioProcessor * processor)
		: Task(processor) {

		filterSet.add(new LowShelfFilter(processor));
		filterSet.add(new PeakFilter(processor));
		filterSet.add(new PeakFilter(processor));
		filterSet.add(new HighShelfFilter(processor));

		setFilterFrequency(1000, LOW, false);
		setFilterGain(2, LOW, false);
		setFilterQ(0.71, LOW, false);

		setFilterFrequency(2000, PEAK1, false);
		setFilterGain(2, PEAK1, false);
		setFilterQ(4, PEAK1, false);

		setFilterFrequency(3000, PEAK2, false);
		setFilterGain(2, PEAK2, false);
		setFilterQ(4, PEAK2, false);

		setFilterFrequency(4000, HIGH, false);
		setFilterGain(2, HIGH, false);
		setFilterQ(0.71, HIGH, false);
	}

	void Equalizer::exec(juce::AudioSampleBuffer& ir) {
		updateFilters();

		for (int i = 0; i < filterSet.size(); i++) {
			filterSet[i]->exec(ir);
		}
	}

	void Equalizer::updateFilters() {

		//Update filter parameters before gain normalization

		const int dim = filterSet.size();

		for (int i = 0; i < filterSet.size(); i++) {
			filterSet[i]->setFrequency(parameters.frequencySet[i]);
			filterSet[i]->setGain(parameters.gainSet[i]);
			filterSet[i]->setQ(parameters.QSet[i]);
		}

		float * evalFrequencies = new float[dim];

		//Set evaluation frequencies

		evalFrequencies[0] = 0;

		for (int i = 1; i < dim - 1; i++) {
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

		for (int i = 0; i < filterSet.size(); i++) {
			gamma_data[i] = filterSet[i]->getdBAmplitude(evalFrequencies[i]);
		}

		//Set gains to 1 dB for B matrix
		for (int i = 0; i < filterSet.size(); i++) {
			parameters.gainSet[i] = Filter::invdB(1.0f);
			filterSet[i]->setGain(Filter::invdB(1.0f));
		}
		


		//Correction algorithm (5 iterations)

		for (int k = 0; k < 5; k++) {

			memcpy(lambda_data, gamma_data, dim * sizeof(float));

			//Compute B matrix with new gains
			for (int i = 0; i < dim; i++) {

				for (int j = 0; j < filterSet.size(); j++) {

					B_data[j + B.getNumColumns() * i] = filterSet[j]->getdBAmplitude(evalFrequencies[i]);
				}
			}

			B.solve(lambda);

			for (int i = 0; i < dim; i++) {

				parameters.gainSet[i] = Filter::invdB(lambda_data[i] * Filter::todB(parameters.gainSet[i]));
				filterSet[i]->setGain(parameters.gainSet[i]);
			}
		}

		delete[] evalFrequencies;


	}

	float Equalizer::getdBAmplitude(float freq) {
		float dBAmplitude = 0;

		for (int i = 0; i < filterSet.size(); i++) {
			dBAmplitude += filterSet[i]->getdBAmplitude(freq);
		}

		return dBAmplitude;

	}

	void Equalizer::setFilterFrequency(float freq, int num, bool update) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();

			parameters.frequencySet[num] = freq;

			if (update)
				updateFilters();
	}
	void Equalizer::setFilterGain(float gain, int num, bool update) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();

			parameters.gainSet[num] = gain;

			if (update)
				updateFilters();

	}
	void Equalizer::setFilterQ(float Q, int num, bool update) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();

			parameters.QSet[num] = Q;
			
			if(update)
				updateFilters();
	}
}