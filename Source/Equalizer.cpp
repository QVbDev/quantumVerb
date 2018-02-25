/*
==============================================================================

Equalizer.cpp

==============================================================================
*/

#include "Equalizer.h"
#define CALIBRATE 1

namespace reverb {

	Equalizer::Equalizer(juce::AudioProcessor * processor)
		: Task(processor) {

		filterSet.add(new LowShelfFilter(processor));
		filterSet.add(new PeakFilter(processor));
		filterSet.add(new PeakFilter(processor));
		filterSet.add(new HighShelfFilter(processor));

		setFilterFrequency(100, LOW);
		setFilterGain(3, LOW);
		setFilterQ(0.71, LOW);

		setFilterFrequency(1000, PEAK1);
		setFilterGain(2, PEAK1);
		setFilterQ(0.5, PEAK1);

		setFilterFrequency(9000, PEAK2);
		setFilterGain(4, PEAK2);
		setFilterQ(0.5, PEAK2);

		setFilterFrequency(15000, HIGH);
		setFilterGain(3, HIGH);
		setFilterQ(0.71, HIGH);

		updateFilters();
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

#if CALIBRATE
		
		juce::dsp::Matrix<float> B(filterSet.size(), filterSet.size());
		juce::dsp::Matrix<float> lambda(filterSet.size(), 1);

		float * B_data = B.getRawDataPointer();
		float * lambda_data = lambda.getRawDataPointer();

		//Compute lambda column

		float gain = 0;

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

				B_data[j + B.getNumColumns() * i] = filterSet[i]->getdBAmplitude(filterSet[j]->frequency);
			}
		}

		//Solve equation
		B.solve(lambda);

		//Set computed gains
		for (int i = 0; i < filterSet.size(); i++) {
			parameters.gainSet[i] = Filter::invdB(lambda_data[i]);
			filterSet[i]->setGain(parameters.gainSet[i]);
		}
		

#endif

	}

	float Equalizer::getdBAmplitude(float freq) {
		float dBAmplitude = 0;

		for (int i = 0; i < filterSet.size(); i++) {
			dBAmplitude += filterSet[i]->getdBAmplitude(freq);
		}

		return dBAmplitude;

	}

	void Equalizer::setFilterFrequency(float freq, int num) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();
		else
			parameters.frequencySet[num] = freq;
	}
	void Equalizer::setFilterGain(float gain, int num) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();
		else
			parameters.gainSet[num] = gain;

	}
	void Equalizer::setFilterQ(float Q, int num) {
		if (num < 0 || num > 3)
			throw InvalidFilterException();
		else
			parameters.QSet[num] = Q;
	}
}