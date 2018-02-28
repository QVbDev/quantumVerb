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

		setFilterFrequency(100, LOW, false);
		setFilterGain(2, LOW, false);
		setFilterQ(0.71, LOW, false);

		setFilterFrequency(1000, PEAK1, false);
		setFilterGain(0.5, PEAK1, false);
		setFilterQ(0.71, PEAK1, false);

		setFilterFrequency(9000, PEAK2, false);
		setFilterGain(3, PEAK2, false);
		setFilterQ(0.71, PEAK2, false);

		setFilterFrequency(15000, HIGH, false);
		setFilterGain(4, HIGH, false);
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

		for (int i = 0; i < filterSet.size(); i++) {
			filterSet[i]->setFrequency(parameters.frequencySet[i]);
			filterSet[i]->setGain(parameters.gainSet[i]);
			filterSet[i]->setQ(parameters.QSet[i]);
		}

		
		juce::dsp::Matrix<float> B(filterSet.size(), filterSet.size());
		juce::dsp::Matrix<float> lambda(filterSet.size(), 1);

		float * B_data = B.getRawDataPointer();
		float * lambda_data = lambda.getRawDataPointer();

		//Compute lambda column

		for (int i = 0; i < filterSet.size(); i++) {

			lambda_data[i] = filterSet[i]->getdBAmplitude(filterSet[i]->frequency);
		}

		//Set gains to 1 dB for B matrix
		for (int i = 0; i < filterSet.size(); i++) {
			filterSet[i]->setGain(Filter::invdB(1.0f));
		}


		//Compute B matrix
		for (int i = 0; i < filterSet.size(); i++) {

			for (int j = 0; j < filterSet.size(); j++) {

				B_data[j + B.getNumColumns() * i] = filterSet[j]->getdBAmplitude(filterSet[i]->frequency);
			}
		}

		//Solve equation
		B.solve(lambda);

		//Set computed gains
		for (int i = 0; i < filterSet.size(); i++) {
			parameters.gainSet[i] = Filter::invdB(lambda_data[i]);
			filterSet[i]->setGain(parameters.gainSet[i]);
		}

		/*
		//Correction algorithm



		for (int i = 0; i < filterSet.size(); i++) {

			lambda_data[i] = 1 / lambda_data[i];
		}

		juce::dsp::Matrix<float> gamma(filterSet.size(), 1);

		float * gamma_data = gamma.getRawDataPointer();

		for (int i = 0; i < filterSet.size(); i++) {

			lambda_data[i] = 1 / lambda_data[i];
		}

		for (int i = 0; i < filterSet.size(); i++) {

			gamma_data[i] = getdBAmplitude(filterSet[i]->frequency);
		}

		lambda = B * lambda*gamma;
		*/
		


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