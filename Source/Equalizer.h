/*
==============================================================================

Equalizer.h

==============================================================================
*/

#pragma once

#include "Task.h"
#include "Filter.h"

#include <memory>
#include <exception>

enum{LOW, PEAK1, PEAK2, HIGH};

namespace reverb {

	class Equalizer : public Task 
	{

	public:

		Equalizer(juce::AudioProcessor * processor, int numFilters = 4);

		virtual void exec(juce::AudioSampleBuffer& ir) override;

		void updateFilters();

		float getdBAmplitude(float freq);

		void setFilterFrequency(float freq, int num);
		void setFilterGain(float gain, int num);
		void setFilterQ(float Q, int num);

		float getFilterFrequency(int num);
		float getFilterGain(int num);
		float getFilterQ(int num);

		float getEQGain(int num);

		int getNumFilters();


	private:
		juce::OwnedArray<Filter> filterSet;
		std::vector<float> EQGains;
	};

	struct InvalidFilterException : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter ID is invalid";
		}
	};

	struct WrongEQFrequency : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter frequency crosses over another one's ";
		}
	};

	struct WrongFilterNumber : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter number must be at least 3 ";
		}
	};
}